#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vastvariables.h"
#include <iostream>

#include <QPainter>
#include <QSettings>
#include <QPointF>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("/home/daniel/Development/VAST-0.4.6/Demo/VASTsim_Qt/VASTsim.INI",
                       QSettings::IniFormat);
    std::cout << "MOVE_MODEL: " << settings.value("MOVE_MODEL").toInt() << std::endl;

    InitPara (VAST_NET_EMULATED, netpara, g_para);
    InitSim (g_para, netpara);
    m_timerId = startTimer(1000);

//    update();
}

void MainWindow::paintEvent(QPaintEvent *event) {

    QPainter painter(this);

    QPen pen(QColor(255,255,0));
    pen.setWidth(1);

//    painter.setWindow(QRect(-50, -50, 100, 100));
    painter.setWindow(QRect(g_origin.x(), g_origin.y(), SCREEN_DIMENSION_X, SCREEN_DIMENSION_Y));

    painter.setPen(pen);
    QRect rect(-40,-40,80,80);
    painter.drawRect(rect);

    pen.setColor(QColor(255,0,255));
    painter.setPen(pen);
    painter.drawEllipse(QPointF(0,0), 100,100);

    pen.setColor(QColor(0,255,255));
    painter.setPen(pen);
    painter.drawLine(QPointF(10,10), QPointF(-10,5));

    int n = g_nodes.size ();

    Node *self = NULL;

    // auto adjust selected node as 1st node if selected node's not found
    if (g_nodes.find (g_selected) == g_nodes.end ())
        g_selected = 0;

    // reset origin for follow mode
    if (follow_mode)
    {
        self = GetNode (g_selected);
        g_origin.setX(-(self->aoi.center.x - 400));
        g_origin.setY(-(self->aoi.center.y - 300));
    }

    painter.setWindow(QRect(g_origin.x(), g_origin.y(), SCREEN_DIMENSION_X, SCREEN_DIMENSION_Y));

    char str[160];

    int size_AN = 0;
    int selected_x = 0 , selected_y =0;

    QPen hPenRed(QBrush(QColor(255,0,0)), 2);
    QPen hPenBlue(QBrush(QColor(0,0,255)), 2);
    QPen hPenGreen(QBrush(QColor(0,255,0)), 2);
    QPen hPenOld;

    // reset # of active (not failed) nodes
    g_nodes_active = 0;

    int missing_count = 0;
    for (int i=0; i<n; ++i)
    {
        self = GetNode (i);

        // we do not display failed nodes
        if (self == NULL)
            continue;

        // g_nodes is not yet obtained before 1st step is run
        if (g_nodes[i] == 0)
            break;

        g_nodes_active++;
        vector<Node *>&nodes = *g_nodes[i];
        vector<Vast::id_t> *en_list = g_ENs[i];

        /*
        // TODO: BUG: this is to prevent program crash under FO model, need to check out
        //            this shouldn't be 0 after initializion is completed
        if (nodes.size () == 0)
        {
            missing_count++;
            continue;
        }
        */

        int     x = (int)self->aoi.center.x;
        int     y = (int)self->aoi.center.y;
        length_t aoi = self->aoi.radius;

        // draw big circle

        /*
                //SelectObject (hdc, GetStockObject(GRAY_BRUSH));
                if (j == 0 || is_EN (en_list, id))
                    hPenOld = (HPEN)SelectObject (hdc, hPenRed);
                else
                    hPenOld = (HPEN)SelectObject (hdc, hPenBlue);
        */


//        SelectObject (hdc, GetStockObject(HOLLOW_BRUSH));

        //SelectObject (hdc, GetStockObject(DC_PEN));
        //SetDCPenColor (hdc, RGB(0x00,0xff,0x00));
        //hPenOld = (HPEN)SelectObject (hdc, hPen);
        //SelectObject( hdc, GetStockObject(BLACK_PEN) );

        painter.drawEllipse (x-NODE_RADIUS, y-NODE_RADIUS, x+NODE_RADIUS, y+NODE_RADIUS);
        //SelectObject (hdc, hPenOld);
        //DeleteObject (hPen);

//        SelectObject (hdc, hPenOld);

        // draw node id
        if (show_node_id)
        {
            //sprintf (str, "%d", (int)VASTnet::resolvePort (self->id)-GATEWAY_DEFAULT_PORT+1);
            std::sprintf (str, "%d", i+1);
            painter.drawText(x-5, y-25, QString(str));
        }

        // draw node AOI
        if (g_show_aoi[i] == true)
        {
            // change color ?
//            SelectObject (hdc, GetStockObject(HOLLOW_BRUSH));
            painter.drawEllipse (x-aoi, y-aoi, x+aoi, y+aoi);
        }

        if (i == g_selected)
        {
            int j;

            g_aoi = self->aoi.radius;
            g_id  = self->id;
            selected_x = x;
            selected_y = y;
            size_AN = nodes.size ()-1;

            // draw AOI
            if (show_self)
            {
                //SelectObject( hdc, GetStockObject(HOLLOW_BRUSH) );
                painter.drawEllipse (x-aoi, y-aoi, x+aoi, y+aoi);
            }

            // draw neighbor dots
            for (j=0; j<(int)nodes.size (); j++)
            {
                int x = (int)nodes[j]->aoi.center.x;
                int y = (int)nodes[j]->aoi.center.y;
                Vast::id_t id = nodes[j]->id;

                //SelectObject(hdc,GetStockObject(DC_PEN));
                //SetDCPenColor(hdc,RGB(0x00,0xff,0x00));

                // draw small circle
                if (show_self)
                {
                    //SelectObject (hdc, GetStockObject(GRAY_BRUSH));
                    if (j == 0 || is_EN (en_list, id))
//                        hPenOld = (HPEN)SelectObject (hdc, hPenRed);
                        hPenOld = hPenRed;
                    else
//                        hPenOld = (HPEN)SelectObject (hdc, hPenBlue);
                        hPenOld = hPenBlue;
                    painter.setPen (hPenOld);
                    painter.drawEllipse (x-(NODE_RADIUS/2), y-(NODE_RADIUS/2), x+(NODE_RADIUS/2), y+(NODE_RADIUS/2));
                }

                // draw node id
                if (show_node_id)
                {
                    //sprintf (str, "%d", (int)VASTnet::resolvePort (nodes[j]->addr.host_id)-GATEWAY_DEFAULT_PORT+1);
                    Vast::id_t neighbor_id = nodes[j]->id;

                    /*
                    int strip = (int)(0x000000000000FFFF & neighbor_id);
                    strip = (int)(0x00000000FFFF0000 & neighbor_id);
                    strip = (int)((0x0000FFFF00000000 & neighbor_id) >> 32);
                    strip = (int)((0xFFFF000000000000 & neighbor_id) >> 32);
                    strip = (int)((0x0000FFFFFFFF0000 & neighbor_id) >> 16);
                    */

                    int index = (g_id2index.find (neighbor_id) == g_id2index.end () ? 0 : g_id2index[neighbor_id]);

                    std::sprintf (str, "%d", index + 1);
                    painter.drawText(x-5, y-25, QString(str));
                }
            }

            // draw Voronoi edges
            if (show_edges && GetEdges (i) != NULL)
            {
                vector<line2d> &lines = *GetEdges (i);
                for (j=0; j<(int)lines.size (); j++)
                {
                    QPointF points[2];

                    points[0].setX(lines[j].seg.p1.x);
                    points[0].setY(lines[j].seg.p1.y);
                    points[1].setX(lines[j].seg.p2.x);
                    points[1].setY(lines[j].seg.p2.y);

                    painter.drawPolyline(points, 2);
                }
            }

            point2d min, max;

            // draw bounding box
            if (show_box && GetBoundingBox (i, min, max) == true)
            {
                vector<line2d> lines;
                lines.push_back (line2d (min.x, min.y, max.x, min.y));
                lines.push_back (line2d (max.x, min.y, max.x, max.y));
                lines.push_back (line2d (max.x, max.y, min.x, max.y));
                lines.push_back (line2d (min.x, max.y, min.x, min.y));

                for (j=0; j<(int)lines.size (); j++)
                {
                    QPointF points[2];

                    points[0].setX(lines[j].seg.p1.x);
                    points[0].setY(lines[j].seg.p1.y);
                    points[1].setX(lines[j].seg.p2.x);
                    points[1].setY(lines[j].seg.p2.y);

                    painter.drawPolyline(points, 2);
                }
            }
        }
    }

    // info line
    char str2[40];
    if (g_inconsistent != (-1))
        std::sprintf (str2, "Inconsistent: %d", g_inconsistent);
    else
        std::sprintf (str2, "[END]");

    std::sprintf (str, "step: %d (%f, %f) node: %d/%d [%d, %d] aoi: %d AN: %d %s %s %s missing: %d",
             g_steps,
             g_cursor.x()-g_origin.x(),
             g_cursor.y()-g_origin.y(),
             (int)VASTnet::resolvePort (g_id)-GATEWAY_DEFAULT_PORT+1,
             g_nodes_active,
             selected_x,
             selected_y,
             g_aoi,
             size_AN,
             str2,
             (follow_mode ? "[follow]" : ""),
             (step_mode ? "[stepped]" : ""),
             missing_count);

    painter.drawText (10-g_origin.x(), 10-g_origin.y(), QString(str));

    // EndPaint balances off the BeginPaint call.
    // EndPaint (hWnd, &ps);
}

void MainWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_timerId) {
        MoveOnce();
        update();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
