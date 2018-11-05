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

    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {

        //Skip subfolders
        if (is_directory(itr->path())) continue;

        std::cout << itr->path() << std::endl;
        std::string filename = itr->path().string();
        //If this is not a TXT file, it is probably not a VASTStatLog file
        if (filename.find(".txt") == string::npos)
        {
            std::cout << "Skipping " << filename << std::endl;
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
          std::cout << "Starting timestamp: " << allRestoredLogs[logIDs[log_iter]].getTimestamp() << std::endl;
          if (allRestoredLogs[logIDs[log_iter]].getTimestamp() < latest_timestamp)
          {
              latest_timestamp = allRestoredLogs[logIDs[log_iter]].getTimestamp();
          }


        }

    }

    std::cout << "First timestamp: " << latest_timestamp << std::endl;

    ofs << "timestamp" << "active_nodes" << "AN_actual" << "AN_visible"
        << "Total drift" << "Max drift" << "drift nodes" << std::endl;

    update();

}

void MainWindow::nextTimestep() {

    //Check all restoredLogs, not just the last one!
    bool finished = true;
    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        finished = finished && allRestoredLogs[logIDs[log_iter]].finished();
    }
    if (finished) {
        killTimer(m_timerId);
        std::cout << "Stopping timer" << std::endl;
    }

}

void MainWindow::paintEvent(QPaintEvent * /*event*/) {

    QPainter painter(this);

    QPen pen(QColor(255,255,0));
    pen.setWidth(1);
    painter.setPen(pen);

    //Drift distance and topology consistency
    size_t AN_actual =0, AN_visible =0, total_drift =0, max_drift =0, drift_nodes =0, active_nodes =0;


    //VASTStatLog approach - instead of working with vectors of entries
    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        painter.setPen(nodeColors[log_iter%nodeColors.size()]);

        VASTStatLog &restoredLog = allRestoredLogs[logIDs[log_iter]];

        //If the log entries are finished, skip
        if (restoredLog.finished())
            continue;

        std::cout << "Plotting node: " << logIDs[log_iter] << std::endl;

        //Get client node state
        Node node = restoredLog.getClientNode();

        std::cout << "timestamp: " << restoredLog.getTimestamp() << std::endl;
        std::cout << "latest timestamp: " << latest_timestamp << std::endl;

        //Check if log entry indexes should be moved along
        if (restoredLog.getTimestamp() <= latest_timestamp + 1 && !restoredLog.finished())
        {
            restoredLog.nextStep();
            latest_timestamp = restoredLog.getTimestamp();
            node = restoredLog.getClientNode();
        }

        std::cout << "Neighbors: " << std::endl;
        for (size_t i =0; i < restoredLog.getNeighbors().size(); i++)
        {
            //Print neighbor for debugging purposes
            std::cout << restoredLog.getNeighbors()[i].id << std::endl;

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

        std::cout << std::endl;


        //Check if the timestamp has caught up with the log entries yet
        if (restoredLog.getFirstTimestamp() <= latest_timestamp) {

            if (restoredLog.getClientNode().id == activeNode)
            {
                //Draw AOI
                painter.drawEllipse(QPointF(node.aoi.center.x, node.aoi.center.y), node.aoi.radius, node.aoi.radius);
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

            //Count the number of active nodes at the moment
            active_nodes++;
        }

        calc_consistency(restoredLog, AN_actual, AN_visible, total_drift, max_drift, drift_nodes);

    }

    painter.setPen(QColor(255,255,255));
    painter.drawText(0, 30, QString("Topo consistency(ANs): %1 / %2").arg(AN_actual).arg(AN_visible));
    painter.drawText(180, 30, QString("Total drift: %1, max drift: %2, drift nodes %3").arg(total_drift).arg(max_drift).arg(drift_nodes));
    painter.drawText(430, 30, QString("Active nodes %1").arg(active_nodes));


    ofs << latest_timestamp << "," << active_nodes << "," << AN_actual <<
           "," << AN_visible << "," << total_drift << "," << max_drift << ","
        << drift_nodes << std::endl;



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

void MainWindow::calc_consistency (const Vast::VASTStatLog &restoredLog, size_t &AN_actual,
                                   size_t &AN_visible, size_t &total_drift, size_t &max_drift,
                                   size_t &drift_nodes)
{
    AN_actual = AN_visible = total_drift = max_drift = drift_nodes = 0;

    // loop through all nodes
    for (size_t j=0; j< allRestoredLogs.size (); ++j)
    {
        const VASTStatLog &otherLog = allRestoredLogs[logIDs[j]];

        // skip self-check or failed / not yet joined node
        if (restoredLog.getClientNode().id == otherLog.getClientNode().id || otherLog.isJoined () == false)
            continue;

        // visible neighbors
        if (restoredLog.in_view (otherLog.getEntry()))
        {
            AN_actual++;

            if (restoredLog.knows (otherLog.getEntry()))
            {
                AN_visible++;

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
        std::cout << counter++ << std::endl;
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
    killTimer(m_timerId);
}
