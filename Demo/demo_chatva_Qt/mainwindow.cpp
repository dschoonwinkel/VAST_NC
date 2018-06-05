#include "mainwindow.h"
#include <iostream>

#include <QtWidgets>
#include <QPainter>
#include <QSettings>
#include <QPointF>
#include <cstring>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), g_netpara(VAST_NET_ACE)
{
    ui->setupUi(this);

    g_origin.setX(0);
    g_origin.setY(0);

    //Store the default gateway address
    char GWstr[80];
    strcpy (GWstr, "127.0.0.1:1037");

    if ((g_node_no = InitPara (VAST_NET_ACE, g_netpara, g_simpara, NULL, &is_gateway, &g_world_id, &g_aoi, GWstr)) == (-1))
    {
        exit(0);
    }

    std::cout << "Starting pos: " << g_aoi.center.x << ":" << g_aoi.center.y << std::endl;

    m_timerId = startTimer(TIMER_INTERVAL);

    setFixedSize(DIM_X, DIM_Y);
    ui->chatInput->move(10, DIM_Y - 75);
    ui->chats->move(10, DIM_Y - 150);

    setUpColors();

    g_world = new VASTVerse(is_gateway, GWstr, &g_netpara, NULL);
    g_world->createVASTNode(1, g_aoi, VAST_EVENT_LAYER);

    connect( ui->chatInput, SIGNAL(returnPressed()), this, SLOT(returnPressed()));


//    update();

}

void MainWindow::nextTimestep() {

    if (!finished)
    {
        g_steps++;

        // NOTE that we will process for as long as needed
        // (so possibly will run out of the time budget)
        g_world->tick (0);

        if (g_state == JOINED)
        {
            MoveOnce ();
        }
        else
        {
            CheckJoin ();
            // update window title if we've just joined
            if (g_state == JOINED)
            {
                // reset window
                char title[80];
                sprintf (title, "Chatva [%d] - tiny chat based on VAST", (int)g_self->getSelf ()->id);
                setWindowTitle(title);
                // replace gateway IP with detected

            }

        }
    }
    update();
}

void MainWindow::paintEvent(QPaintEvent *event) {

    QPainter painter(this);

    QPen pen(QColor(255,255,255));
    pen.setWidth(1);

    if (g_self == NULL)
    {
        return;
    }

    Node *self = g_self->getSelf ();
    if (self == NULL)
    {
        return;
    }

    vector<Node*> &nodes = g_self->list();
    g_Voronoi = g_world->getMatcherVoronoi();

    char str[160];
    int n = nodes.size();

//    painter.setPen(QColor(255,255,255));
    painter.setPen(nodeColors[self->id % 20]);
    painter.drawEllipse(QPointF(g_aoi.center.x, g_aoi.center.y), NODE_RADIUS, NODE_RADIUS);
    painter.drawEllipse(QPointF(g_aoi.center.x, g_aoi.center.y), g_aoi.radius, g_aoi.radius);

    // draw matcher AOI, if available
    Area *matcher_aoi = g_world->getMatcherAOI ();
    if (matcher_aoi != NULL)
    {
        painter.setPen(QColor(255,0,0));
        painter.drawEllipse(QPointF(matcher_aoi->center.x, (int)matcher_aoi->center.y), matcher_aoi->radius, matcher_aoi->radius);
    }

    size_t j;

    // draw neighbor dots
    for(j=0; j< nodes.size (); j++)
    {
        //int size = nodes.size ();
        int   x = (int)nodes[j]->aoi.center.x;
        int   y = (int)nodes[j]->aoi.center.y;
        Vast::id_t id = nodes[j]->id;
        //id_t id = nodes[j]->addr.host_id;

        painter.setPen(nodeColors[id % 20]);

        // draw small circle
        painter.drawEllipse (QPoint(x,y), (NODE_RADIUS/2), (NODE_RADIUS/2));

        // draw node id
        sprintf (str, "%d", (int)VASTnet::resolvePort (id) - g_netpara.port + 1);
        painter.drawText(QPointF(x-5, y-25), QString(str));
    }

    // draw Voronoi edges
    if (show_edges && g_Voronoi != NULL)
    {
        vector<line2d> &lines = g_Voronoi->getedges ();
        for (j=0; j< lines.size (); j++)
        {
            QPointF points[2];

            points[0].setX(lines[j].seg.p1.x);
            points[0].setY(lines[j].seg.p1.y);
            points[1].setX(lines[j].seg.p2.x);
            points[1].setY(lines[j].seg.p2.y);

            painter.drawLine(points[0], points[1]);
        }
    }

    // info line
    char join_state[80];
    if (g_state == ABSENT)
        strcpy (join_state, "[ABSENT]");
    else if (g_state == JOINED)
        strcpy (join_state, "[JOINED]");
    else
        strcpy (join_state, "[JOINING]");

    string GWstr;
    g_gateway.toString (GWstr);

    painter.setPen(QColor(255,255,255));
    sprintf (str, "step: %d node: %d [%d, %d] %s aoi: %d CN: %d %s char: %d gw: %s",
             g_steps/(UPDATE_INTERVAL/TIMER_INTERVAL),
             (int)VASTnet::resolvePort (self->id)-g_netpara.port+1, (int)g_aoi.center.x, (int)g_aoi.center.y,
             join_state,
             (matcher_aoi == NULL ? self->aoi.radius : matcher_aoi->radius),
             n, (follow_mode ? "[follow]" : ""),
             last_char,
             GWstr.c_str ());

    painter.drawText(QPoint(0, 30), QString(str));


}

void MainWindow::MoveOnce ()
{

    if (finished == true)
        return;

    // we don't want to move too fast (not as fast as frame rate)
    if (g_steps % (UPDATE_INTERVAL/TIMER_INTERVAL) == 0)
    {
        g_self->move (g_sub_no, g_aoi);
    }

    // check and store any incoming text messages
    char recv_buf[INPUT_SIZE];
    size_t size = 0;
    Vast::id_t from;

    do
    {
        Message *msg = NULL;
        if ((msg = g_self->receive ()) != NULL)
        {
//            size = msg->extract (from);
            from = msg->from;
            size = msg->extract (recv_buf, 0);
            recv_buf[size]=0;
        }
        else
            size = 0;

        if (size > 0)
        {
            string chatmsg (recv_buf, size);
            chatmsg = std::to_string(from) + " : " + chatmsg;
            StoreChat (chatmsg);
        }
    }
    while (size > 0);

    update();
}

bool MainWindow::StoreChat (std::string &msg)
{
    if (msg.size () == 0)
        return false;

    // TODO: we never release the allocated chat msg
    g_chatmsg.push_back (msg);

    ui->chats->clear();
    // draw chat text
    unsigned int n = g_chatmsg.size() > SIZE_TEXTAREA ? g_chatmsg.size() - SIZE_TEXTAREA : 0;
    for (; n < g_chatmsg.size(); n++) {
        string &chat = g_chatmsg[n];
        ui->chats->append(chat.c_str ());
    }

    return true;
}

void MainWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_timerId) {
//        printf("Timestep\n");
        nextTimestep();
        update();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Down) {
        g_aoi.center.y += 5;
    }

    if (event->key() == Qt::Key_Up) {
        g_aoi.center.y -= 5;
    }

    if (event->key() == Qt::Key_Left) {
        g_aoi.center.x -= 5;
    }

    if (event->key() == Qt::Key_Right) {
        g_aoi.center.x += 5;
    }

    update();
}

void MainWindow::returnPressed()
{
    // send off input to AOI neighbors
    // note: self is also included as the 1st neighbor
    Message send_msg (12);

    send_msg.clear (123);
    QString chat_text = ui->chatInput->text();
    send_msg.store(g_self->getSelf()->id);
    send_msg.store (chat_text.toStdString().c_str(), chat_text.length(), true);

    ui->chatInput->setText("");

    /* SEND-based */
    vector<Node *>&nodes = g_self->list ();
    //vector<Node *>&nodes = g_self->getLogicalNeighbors ();
    //vector<Node *>&nodes = g_self->getPhysicalNeighbors ();

    for (size_t j=0; j < nodes.size (); j++)
    {
        Position pos = nodes[j]->aoi.center;
        Vast::id_t id      = nodes[j]->id;
        send_msg.addTarget (id);
    }

    // send away message
    g_self->send (send_msg);


    // PUBLISH-based
    //g_self->publish (g_aoi, VAST_EVENT_LAYER, send_msg);

}

// make sure we've joined the network
void MainWindow::CheckJoin ()
{
    if (g_state == ABSENT)
    {
        if ((g_self = g_world->getVASTNode ()) != NULL)
        {
            g_sub_no = g_self->getSubscriptionID ();
            g_state = JOINED;
        }
    }
}

void MainWindow::setUpColors() {

    nodeColors.push_back(QColor(230, 25, 75));
    nodeColors.push_back(QColor(60, 180, 75));
    nodeColors.push_back(QColor(255, 225, 25));
    nodeColors.push_back(QColor(0, 130, 200));
    nodeColors.push_back(QColor(245, 130, 48));
    nodeColors.push_back(QColor(145, 30, 180));
    nodeColors.push_back(QColor(70, 240, 240));
    nodeColors.push_back(QColor(240, 50, 230));
    nodeColors.push_back(QColor(210, 245, 60));
    nodeColors.push_back(QColor(250, 190, 190));
    nodeColors.push_back(QColor(0, 128, 128));
    nodeColors.push_back(QColor(230, 190, 255));
    nodeColors.push_back(QColor(170, 110, 40));
    nodeColors.push_back(QColor(255, 250, 200));
    nodeColors.push_back(QColor(128, 0, 0));
    nodeColors.push_back(QColor(170, 255, 195));
    nodeColors.push_back(QColor(128, 128, 0));
    nodeColors.push_back(QColor(255, 215, 180));
    nodeColors.push_back(QColor(0, 0, 128));
    nodeColors.push_back(QColor(128, 128, 128));
}

MainWindow::~MainWindow()
{
    delete ui;
    finished = true;
    if (g_self != NULL)
    {
        g_self->leave();
    }
    g_world->tick(0);
    g_world->destroyVASTNode(g_self);
    delete g_world;
}
