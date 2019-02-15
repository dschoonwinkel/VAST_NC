#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>

#include <QPainter>
#include <QSettings>
#include <QPointF>
#include <QKeyEvent>

#define TIMERINTERVAL 25
#define FASTTIMERINTERVAL 0
#define NEIGHBOR_LINES
#define PLOT_RESULTS



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef PLOT_RESULTS
    m_timerId = startTimer(TIMERINTERVAL);
#else
    m_timerId = startTimer(FASTTIMERINTERVAL);
#endif


    setFixedSize(DIM_X, DIM_Y);
    setUpColors();

    CPPDEBUG("MainWindow::Starting VASTStatReplayQt"<<std::endl);

    initVariables();

    CPPDEBUG("MainWindow::First timestamp: " << latest_timestamp << std::endl);

    process_duration = std::chrono::microseconds::zero();


    update();

}

void MainWindow::nextTimestep() {

    //Check all restoredLogs, not just the last one!
    finished = true;
    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        finished = finished && allRestoredLogs[logIDs[log_iter]].finished();
    }
    if (finished) {
        killTimer(m_timerId);
        CPPDEBUG("MainWindow::nextTimestep::Stopping timer" << std::endl);

#ifndef PLOT_RESULTS
      exit(0);
#endif
    }

    calculateUpdate();


}

void MainWindow::paintEvent(QPaintEvent * /*event*/) {

#ifdef PLOT_RESULTS
    QPainter painter(this);

    QPen pen(QColor(255,255,0));
    pen.setWidth(1);
    painter.setPen(pen);

    //VASTStatLog approach - instead of working with vectors of entries
    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        painter.setPen(nodeColors[log_iter%nodeColors.size()]);

        VASTStatLog &restoredLog = allRestoredLogs[logIDs[log_iter]];

        //If the log entries are finished, skip
        if (restoredLog.finished())
            continue;

//        CPPDEBUG("\nMainWindow::Plotting node: " << logIDs[log_iter] << std::endl);

        //Get client node state
        Node node = restoredLog.getClientNode();

//        CPPDEBUG("MainWindow::paintEvent::timestamp: " << restoredLog.getTimestamp() << std::endl);
//        CPPDEBUG("MainWindow::paintEvent::latest timestamp: " << latest_timestamp << std::endl);

        //Only print neighbors if the node has already joined
        if (restoredLog.isJoinedAt(latest_timestamp))
        {
//            CPPDEBUG("MainWindow::Neighbors: " << std::endl);
            for (size_t i =0; i < restoredLog.getNeighbors().size(); i++)
            {
                //Print neighbor for debugging purposes
//                CPPDEBUG("VASTStatReplayQt::paintEvent neighbour id: "<< restoredLog.getNeighbors()[i].id << std::endl << std::endl);

                //Check if I know the neighbors around me
                for (size_t j = 0; j < logIDs.size(); j++)
                {
                    //Skip myself
                    if (allRestoredLogs[logIDs[j]].getClientNode().id == restoredLog.getClientNode().id)
                        continue;


    //                std::cout << "Does " << restoredLog.getClientNode().id << " know "
    //                          << allRestoredLogs[logIDs[j]].getClientNode().id << ": "
    //                          << restoredLog.knows(allRestoredLogs[logIDs[j]].getEntry())
    //                          << std::endl;
                }

                //Check if I know the neighbors around me
                for (size_t j = 0; j < logIDs.size(); j++)
                {
                    //Skip myself
                    if (allRestoredLogs[logIDs[j]].getClientNode().id == restoredLog.getClientNode().id)
                        continue;

    //                std::cout << "Is " << restoredLog.getClientNode().id << " in  range of "
    //                          << allRestoredLogs[logIDs[j]].getClientNode().id << "?: "
    //                          << restoredLog.knows(allRestoredLogs[logIDs[j]].getEntry())
    //                          << std::endl;
                }


            }

//            CPPDEBUG("" << std::endl);
        }


        //Check if the timestamp has caught up with the log entries yet
        if (restoredLog.getFirstTimestamp() <= latest_timestamp) {

            if (restoredLog.getClientNode().id == activeNode)
            {
                //Draw AOI
                painter.drawEllipse(QPointF(node.aoi.center.x, node.aoi.center.y), node.aoi.radius, node.aoi.radius);

                //Calc consistency for active node
                size_t active_AN_actual = 0, active_AN_visible = 0, active_total_drift = 0, active_max_drift,
                        active_drift_nodes = 0;

                calc_consistency(restoredLog, active_AN_actual, active_AN_visible, active_total_drift,
                                 active_max_drift, active_drift_nodes, latest_timestamp);

                painter.drawText(0, 50, QString("Active node: (ANs): %1 / %2   Total drift: %3, drift nodes %5")
                                 .arg(active_AN_actual).arg(active_AN_visible).arg(active_total_drift).arg(active_drift_nodes));
            }

            //Just draw center
            painter.drawEllipse(QPointF(node.aoi.center.x, node.aoi.center.y), 10,10);
            painter.drawText(node.aoi.center.x, node.aoi.center.y, QString("%1").arg(node.id % 1000));

            //Draw neighbor centers
            for (size_t j = 0; j < restoredLog.getNeighbors().size(); j++)
            {
                Node &neighbor = restoredLog.getNeighbors()[j];
                coord_t x_offset = neighbor.aoi.center.x;
                coord_t y_offset = neighbor.aoi.center.y;

                const coord_t cross_size = 3;

                painter.drawLine(-cross_size + x_offset, -cross_size + y_offset, cross_size + x_offset,   cross_size + y_offset);
                painter.drawLine(-cross_size + x_offset,  cross_size + y_offset, cross_size + x_offset,   -cross_size + y_offset);
//                painter.drawEllipse(QPointF(neighbor.aoi.center.x, neighbor.aoi.center.y), 1,1);
            }

#ifdef NEIGHBOR_LINES
            //Draw lines to neighbors - only for debugging purposes
            for (size_t j = 0; j < restoredLog.getNeighbors().size(); j++)
            {
                Node &neighbor = restoredLog.getNeighbors()[j];
                coord_t x_offset = neighbor.aoi.center.x;
                coord_t y_offset = neighbor.aoi.center.y;


                if (node.aoi.center.distance(neighbor.aoi.center) < (double) node.aoi.radius)
                    painter.drawLine(x_offset,y_offset, node.aoi.center.x,   node.aoi.center.y);
            }

#endif

        }

        //Draw inactive log nodes in grey
        else
        {
            painter.setPen(QPen(QColor(100,100,100)));
            //Just draw center
            painter.drawEllipse(QPointF(node.aoi.center.x, node.aoi.center.y), 10,10);
            painter.drawText(node.aoi.center.x, node.aoi.center.y, QString("%1").arg(node.id % 1000));

        }

        //Draw Matcher & Matcher AOI
        if (restoredLog.getWorldIsMatcher() && restoredLog.getMatcherAOI() != NULL)
        {
            std::cout << "Plotting matcher: x: " << restoredLog.getMatcherAOI()->center.x <<
                            " y: " <<  restoredLog.getMatcherAOI()->center.y <<
                            " radius: " << restoredLog.getMatcherAOI()->radius << std::endl;
            painter.setPen(nodeColors[log_iter%nodeColors.size()]);
            painter.drawRect(restoredLog.getMatcherAOI()->center.x, restoredLog.getMatcherAOI()->center.y, 10, 10);

            painter.setPen(QPen(nodeColors[log_iter%nodeColors.size()], Qt::DashLine));
            painter.drawEllipse(restoredLog.getMatcherAOI()->center.x, restoredLog.getMatcherAOI()->center.y,
                                restoredLog.getMatcherAOI()->radius, restoredLog.getMatcherAOI()->radius);

        }
    }




    painter.setPen(QColor(255,255,255));
    painter.drawText(0, 30, QString("Topo consistency(ANs): %1 / %2   Total drift: %3, max drift: %4, drift nodes %5,   Active nodes %6    Timestamp: %7")
                     .arg(total_AN_actual).arg(total_AN_visible).arg(total_drift).arg(max_drift)
                     .arg(drift_nodes).arg(total_active_nodes).arg(latest_timestamp));


#endif

//    CPPDEBUG("****" << std::endl);
}



void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space)
    {
        paused = !paused;
    }

    if (paused)
    {
        killTimer(m_timerId);
    }
    else {
        m_timerId = startTimer(TIMERINTERVAL);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        lastMouseClickPoint = event->pos();

        for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++)
        {
            VASTStatLog &restoredLog = allRestoredLogs[logIDs[log_iter]];

            if (restoredLog.isJoined())
            {
                //If the click point is within the little circle, make this node the active node
                Vast::Position clickPos(lastMouseClickPoint.x(), lastMouseClickPoint.y());
                if (restoredLog.getClientNode().aoi.center.distance(clickPos) <= 10)
                {
                    activeNode = restoredLog.getClientNode().id;
                    update();
                }
            }
        }
    }

}

void MainWindow::timerEvent(QTimerEvent *event) {
    static size_t counter = 0;

    if (event->timerId() == m_timerId) {

        auto t1 = std::chrono::high_resolution_clock::now();
        nextTimestep();
        update();
        outputResults();

        auto t2 = std::chrono::high_resolution_clock::now();
        process_duration += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);

        if (counter % 200 == 0)
        {
            CPPDEBUG(counter++ << std::endl);
            std::cout << "Average process duration: " << process_duration.count() / counter << " microsec" << std::endl;
        }
        else
            counter++;
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
    closeOutputFile();
    if (!finished)
        killTimer(m_timerId);
}
