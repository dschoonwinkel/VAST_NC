#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

#include <QPainter>
#include <QSettings>
#include <QPointF>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), netpara(VAST_NET_EMULATED)
{
    ui->setupUi(this);

//    QSettings settings("$(HOME)/Development/VAST-0.4.6/Demo/VASTsim_Qt/VASTsim.INI",
//                       QSettings::IniFormat);
//    std::cout << "MOVE_MODEL: " << settings.value("MOVE_MODEL").toInt() << std::endl;

    InitPara (VAST_NET_EMULATED, netpara, simpara);

    InitSim (simpara, netpara);

    m_timerId = startTimer(100);

    setFixedSize(simpara.WORLD_WIDTH, simpara.WORLD_HEIGHT);
    setUpColors();


//    update();

}

void MainWindow::nextTimestep() {

    steps++;
    printf("step %d\n", steps);

    // create nodes
    if (nodes_created < simpara.NODE_SIZE)
    {
        if (create_countdown == 0)
        {
            CreateNode (nodes_created == 0);
            nodes_created++;
            printf("Creating node %d \n%3d %%\n", nodes_created, (int) ((double) (nodes_created * 100) / (double) simpara.NODE_SIZE));
            create_countdown = simpara.JOIN_RATE;
        }
        else
            create_countdown--;
    }

    //If the maximum simulation timesteps have been reached, stop the simulation
    if (NextStep () < 0)
    {
        //Stop the screen updates
        killTimer(m_timerId);
        //        close();
    }

}

void MainWindow::paintEvent(QPaintEvent * /*event*/) {

    QPainter painter(this);

    QPen pen(QColor(255,255,255));
    pen.setWidth(1);

    for (int node_index = 0; node_index < simpara.NODE_SIZE; node_index++)
    {
        pen.setColor(nodeColors[node_index%20]);
        painter.setPen(pen);
        Node* node = GetNode(node_index);
        if (node == NULL)
        {
            break;
        }

        painter.drawText(QPointF(node->aoi.center.x, node->aoi.center.y), QString::number(node_index));
//        printf("Node %d is at pos (%f,%f)\n", node_index, node->aoi.center.x, node->aoi.center.y);
        //Draw AOI
//            painter.drawEllipse(QPointF(node->aoi.center.x, node->aoi.center.y), node->aoi.radius, node->aoi.radius);
        //Just draw center
        painter.drawEllipse(QPointF(node->aoi.center.x, node->aoi.center.y), 1,1);

        if (GetEdges (node_index) != NULL && isNodeMatcher(node_index))
        {
            vector<line2d> &lines = *GetEdges (node_index);
            for (int j=0; j<(int)lines.size (); j++)
            {
                QPointF points[2];

//                pen.setColor(QColor(0,0,0));
//                painter.setPen(pen);

                points[0].setX(lines[j].seg.p1.x);
                points[0].setY(lines[j].seg.p1.y);
                points[1].setX(lines[j].seg.p2.x);
                points[1].setY(lines[j].seg.p2.y);

                painter.drawLine(points[0], points[1]);

//                printf("Lines for node %d: (%2.2f, %2.2f) - (%2.2f, %2.2f)\n", node_index, points[0].x(), points[0].y(), points[1].x(), points[1].y());
            }
//            printf("Number of lines %d\n", (int)lines.size());
            //Draw a rectangle so that we know this is a relay
            painter.drawRect(GetNodeMatcherAOI(node_index)->center.x-10, GetNodeMatcherAOI(node_index)->center.y-10, 20, 20);
        }
        else if (!isNodeMatcher(node_index)) {
//            printf("Node was not a matcher %d\n", node_index);
        }
        else {
            printf("GetEdges returned NULL for node %d\n", node_index);
        }
    }


}

void MainWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_timerId) {
        nextTimestep();
        update();
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

    ShutSim();
}
