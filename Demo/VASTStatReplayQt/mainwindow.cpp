#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>

#include <QPainter>
#include <QSettings>
#include <QPointF>
#include <QKeyEvent>

#include <climits>

#include "boost/filesystem.hpp"
using namespace boost::filesystem;

#define TIMERINTERVAL 25
#define UPDATE_PERIOD 10
#define NEIGHBOR_LINES

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), netpara(VAST_NET_EMULATED), ofs(results_file)
{
    ui->setupUi(this);

    m_timerId = startTimer(TIMERINTERVAL);

    setFixedSize(DIM_X, DIM_Y);
    setUpColors();

    path dir_path("./logs");
    directory_iterator end_itr;

    CPPDEBUG("MainWindow::Starting VASTStatReplayQt"<<std::endl);

    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {

        //Skip subfolders
        if (is_directory(itr->path())) continue;

        CPPDEBUG(itr->path() << std::endl);
        std::string filename = itr->path().string();
        //If this is not a TXT file, it is probably not a VASTStatLog file
        if (filename.find(".txt") == string::npos)
        {
            CPPDEBUG("Skipping " << filename << std::endl);
            continue;
        }
        std::vector<Vast::VASTStatLogEntry> restoredLogs = Vast::VASTStatLogEntry::restoreAllFromLogFile(filename);
        VASTStatLog restoredLog(restoredLogs);

        //Cut off .txt
        std::string id_string = filename.substr(0, filename.find(".txt"));
        //Extract id_t
        id_string = id_string.substr(id_string.find("N") + 1);

        Vast::id_t restoredLogID = stoll(id_string);
        allRestoredLogs[restoredLogID] = restoredLog;
        logIDs.push_back(restoredLogID);

        latest_timestamp = ULONG_LONG_MAX;
        //Initiate latest_timestamp to the earliest timestamp
        for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++)
        {
          CPPDEBUG("Starting timestamp: " << allRestoredLogs[logIDs[log_iter]].getTimestamp() << std::endl);
          if (allRestoredLogs[logIDs[log_iter]].getTimestamp() < latest_timestamp)
          {
              latest_timestamp = allRestoredLogs[logIDs[log_iter]].getTimestamp();
          }


        }

    }

    CPPDEBUG("MainWindow::First timestamp: " << latest_timestamp << std::endl);

    ofs << "timestamp," << "active_nodes," << "AN_actual," << "AN_visible,"
        << "Total drift," << "Max drift," << "drift nodes," << "worldSendStat," << "worldRecvStat," << std::endl;

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
    }

    calculateUpdate();


}

void MainWindow::calculateUpdate() {

    //Drift distance and topology consistency
    total_AN_actual =0, total_AN_visible =0, total_drift =0, max_drift =0, drift_nodes =0;
    worldSendStat = 0, worldRecvStat = 0;


    //VASTStatLog approach - instead of working with vectors of entries
    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        VASTStatLog &restoredLog = allRestoredLogs[logIDs[log_iter]];

        //If the log entries are finished, skip
        if (restoredLog.finished())
            continue;

        //Get client node state
        Node node = restoredLog.getClientNode();

//        CPPDEBUG("MainWindow::calcUpdate:: timestamp: " << restoredLog.getTimestamp() << std::endl);
//        CPPDEBUG("MainWindow::calcUpdate:: latest timestamp: " << latest_timestamp << std::endl);

        //Allow each log to catch up to the current timestamp
        while (restoredLog.getTimestamp() < latest_timestamp && !restoredLog.finished())
        {
            restoredLog.nextStep();
            node = restoredLog.getClientNode();
        }

        if (restoredLog.isJoinedAt(latest_timestamp))
        {
            calc_consistency(restoredLog, total_AN_actual, total_AN_visible, total_drift, max_drift, drift_nodes, latest_timestamp);
            worldSendStat += restoredLog.getWorldSendStat().total;
            worldRecvStat += restoredLog.getWorldRecvStat().total;
        }

        if (restoredLog.getClientNode().id == activeNode)
        {
            //Calculate active node's specific parameters
        }

    }

    latest_timestamp += UPDATE_PERIOD;





}

void MainWindow::paintEvent(QPaintEvent * /*event*/) {

    QPainter painter(this);

    QPen pen(QColor(255,255,0));
    pen.setWidth(1);
    painter.setPen(pen);

    total_active_nodes =0;


    //VASTStatLog approach - instead of working with vectors of entries
    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        painter.setPen(nodeColors[log_iter%nodeColors.size()]);

        VASTStatLog &restoredLog = allRestoredLogs[logIDs[log_iter]];

        //If the log entries are finished, skip
        if (restoredLog.finished())
            continue;

//        CPPDEBUG("MainWindow::Plotting node: " << logIDs[log_iter] << std::endl);

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
//                CPPDEBUG(restoredLog.getNeighbors()[i].id << std::endl);

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

            //Count the number of active nodes at the moment
            total_active_nodes++;
        }

        //Draw inactive log nodes in grey
        else
        {
            painter.setPen(QPen(QColor(100,100,100)));
            //Just draw center
            painter.drawEllipse(QPointF(node.aoi.center.x, node.aoi.center.y), 10,10);
            painter.drawText(node.aoi.center.x, node.aoi.center.y, QString("%1").arg(node.id % 1000));

        }
    }

    painter.setPen(QColor(255,255,255));
    painter.drawText(0, 30, QString("Topo consistency(ANs): %1 / %2   Total drift: %3, max drift: %4, drift nodes %5,   Active nodes %6    Timestamp: %7")
                     .arg(total_AN_actual).arg(total_AN_visible).arg(total_drift).arg(max_drift)
                     .arg(drift_nodes).arg(total_active_nodes).arg(latest_timestamp));
}

void MainWindow::outputResults() {

    ofs << latest_timestamp << "," << total_active_nodes << "," << total_AN_actual <<
           "," << total_AN_visible << "," << total_drift << "," << max_drift << ","
        << drift_nodes << "," << worldSendStat << "," << worldRecvStat << std::endl;
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

void MainWindow::calc_consistency (const Vast::VASTStatLog &restoredLog, size_t &total_AN_actual,
                                   size_t &total_AN_visible, size_t &total_drift, size_t &max_drift,
                                   size_t &drift_nodes, timestamp_t latest_timestamp)
{
//    AN_actual = AN_visible = total_drift = max_drift = drift_nodes = 0;

    // loop through all nodes
    for (size_t j=0; j< allRestoredLogs.size (); ++j)
    {
        const VASTStatLog &otherLog = allRestoredLogs[logIDs[j]];

        // skip self-check or failed / not yet joined node
        if (restoredLog.getClientNode().id == otherLog.getClientNode().id || otherLog.isJoinedAt(latest_timestamp) == false)
        {
            continue;
        }

        // visible neighbors
        if (restoredLog.in_view (otherLog))
        {
            total_AN_actual++;

            if (restoredLog.knows (otherLog))
            {
                total_AN_visible++;

                // calculate drift distance (except self)
                // NOTE: drift distance is calculated for all known AOI neighbors
                drift_nodes++;

                size_t drift = (size_t)restoredLog.getNeighborByID(otherLog.getSubID()).aoi.center.distance (otherLog.getClientNode().aoi.center);
                total_drift += drift;

                if (max_drift < drift)
                {
                    max_drift = drift;
#ifdef DEBUG_DETAIL
                    printf ("%4d - max drift updated: [%d] info on [%d] drift: %d\n", _steps+1, (int)_simnodes[i]->getID (), (int)neighbor->id, (int)drift);
#endif
                }
            }
        }
    } // end looping through all other nodes


}

void MainWindow::timerEvent(QTimerEvent *event) {
    static size_t counter = 0;

    if (event->timerId() == m_timerId) {

        nextTimestep();
        update();
        outputResults();

        if (counter % 200 == 0)
            CPPDEBUG(counter++ << std::endl);
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
    ofs.flush();
    ofs.close();
    if (!finished)
        killTimer(m_timerId);
}
