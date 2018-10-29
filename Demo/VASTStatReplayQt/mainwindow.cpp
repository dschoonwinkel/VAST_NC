#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>

#include <QPainter>
#include <QSettings>
#include <QPointF>

#include <climits>

#include "boost/filesystem.hpp"
using namespace boost::filesystem;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), netpara(VAST_NET_EMULATED)
{
    ui->setupUi(this);

//    QSettings settings("/home/daniel/Development/VAST-0.4.6/Demo/VASTsim_Qt/VASTsim.INI",
//                       QSettings::IniFormat);
//    std::cout << "MOVE_MODEL: " << settings.value("MOVE_MODEL").toInt() << std::endl;

//    InitPara (VAST_NET_EMULATED, netpara, simpara);

//    InitSim (simpara, netpara);

    m_timerId = startTimer(100);

    setFixedSize(DIM_X, DIM_Y);
    setUpColors();

    std::cout << "Hello there!!" << std::endl;

    path dir_path("./logs");
    directory_iterator end_itr;
    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {
        std::cout << itr->path() << std::endl;

    //    std::string filename = "./logs/VASTStat_N9151315546691403777.txt";
//        std::string filename = "./logs/VASTStat_N9151315546691469313.txt";
        std::string filename = itr->path().string();
        restoredLogs = Vast::VASTStatLog::restoreAllFromLogFile(filename);

        //Cut off .txt
        std::string id_string = filename.substr(0, filename.find(".txt"));
        //Extract id_t
        id_string = id_string.substr(id_string.find("N") + 1);

    //    std::cout << id_string << std::endl;
        Vast::id_t restoredLogID = stoll(id_string);
    //    std::cout << "Converted to id_t: " << restoredLogID << std::endl;
        allRestoredLogs[restoredLogID] = restoredLogs;
        logIDs.push_back(restoredLogID);
        //Insert the starting step
        log_steps.push_back(0);

    //    for (Vast::VASTStatLog log : restoredLogs)
    //    {
    //        std::cout << log;
    //    }

        latest_timestamp = ULONG_LONG_MAX;
        //Initiate latest_timestamp to the earliest timestamp
        for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++)
        {
          std::cout << "Stariting timestamp: " << allRestoredLogs[logIDs[log_iter]].at(0).getTimestamp() << std::endl;
          if (allRestoredLogs[logIDs[log_iter]].at(0).getTimestamp() < latest_timestamp)
          {
              latest_timestamp = allRestoredLogs[logIDs[log_iter]].at(0).getTimestamp();
          }


        }

        std::cout << "First timestamp: " << latest_timestamp << std::endl;

    }


    update();

}

void MainWindow::nextTimestep() {

//    steps++;
//    printf("step %lu\n", steps);

//    // create nodes
//    if (nodes_created < simpara.NODE_SIZE)
//    {
//        if (create_countdown == 0)
//        {
//            CreateNode (nodes_created == 0);
//            nodes_created++;
//            printf("Creating node %d \n%3d %%\n", nodes_created, (int) ((double) (nodes_created * 100) / (double) simpara.NODE_SIZE));
//            create_countdown = simpara.JOIN_RATE;
//        }
//        else
//            create_countdown--;
//    }

//    //If the maximum simulation timesteps have been reached, stop the simulation
//    if (NextStep () < 0)
//    {
//        //Stop the screen updates
//        killTimer(m_timerId);
//        //        close();
//    }



    //TODO: Check all restoredLogs, not just the last one!

    bool finished = true;
    for (size_t log_iter = 0; log_iter < log_steps.size(); log_iter++) {

        finished = finished && log_steps[log_iter] >= allRestoredLogs[logIDs[log_iter]].size();
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

    for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++) {

        std::cout << "Plotting node: " << logIDs[log_iter] << std::endl;
        painter.setPen(nodeColors[log_iter%nodeColors.size()]);
        restoredLogs = allRestoredLogs[logIDs[log_iter]];

        //Get client node state
        Node node = restoredLogs[log_steps[log_iter]].getClientNode();

        //Check if log entry indexes should be moved along
        if (log_steps[log_iter] < restoredLogs.size() &&
                restoredLogs[log_steps[log_iter]].getTimestamp() <= latest_timestamp)
        {
            log_steps[log_iter]++;
            latest_timestamp = restoredLogs[log_steps[log_iter]].getTimestamp();
            Node node = restoredLogs[log_steps[log_iter]].getClientNode();
        }


        std::cout << "Neighbors: " << std::endl;
        for (size_t i =0; i < restoredLogs[log_steps[log_iter]].getNeighbors().size(); i++)
        {
            //Print neighbor for debugging purposes
            std::cout << restoredLogs[log_steps[log_iter]].getNeighbors()[i].id << std::endl;

            //Check if I know the neighbors around me
            for (size_t j = 0; j < logIDs.size(); j++)
            {

            }


        }
        std::cout << std::endl;


        //Check if the timestamp has caught up with the log entries yet
        if (restoredLogs[0].getTimestamp() <= latest_timestamp) {
            //Draw AOI
            painter.drawEllipse(QPointF(node.aoi.center.x, node.aoi.center.y), node.aoi.radius, node.aoi.radius);
            //Just draw center
            painter.drawEllipse(QPointF(node.aoi.center.x, node.aoi.center.y), 1,1);
        }
    }




}

//void calc_consistency (size_t i, size_t &AN_actual, size_t &AN_visible, size_t &total_drift, size_t &max_drift, size_t &drift_nodes)
//{
//    size_t n = _simnodes.size ();
//    Node *neighbor;
//    AN_actual = AN_visible = total_drift = max_drift = drift_nodes = 0;

//    // loop through all nodes
//    for (size_t j=0; j<n; ++j)
//    {
//        // skip self-check or failed / not yet joined node
//#ifdef STAT_JOINED_NODE_ONLY
//        if (i == j || _simnodes[j]->isJoined () == false)
//#else
//        if (i == j || _simnodes[j]->isFailed ())
//#endif
//            continue;

//        // visible neighbors
//        if (_simnodes[i]->in_view (_simnodes[j]))
//        {
//            AN_actual++;

//            if ((neighbor = _simnodes[i]->knows (_simnodes[j])) != NULL)
//            {
//                AN_visible++;

//                // calculate drift distance (except self)
//                // NOTE: drift distance is calculated for all known AOI neighbors
//                drift_nodes++;

//                size_t drift = (size_t)neighbor->aoi.center.distance (_simnodes[j]->get_pos ());
//                total_drift += drift;

//                if (max_drift < drift)
//                {
//                    max_drift = drift;
//#ifdef DEBUG_DETAIL
//                    printf ("%4d - max drift updated: [%d] info on [%d] drift: %d\n", _steps+1, (int)_simnodes[i]->getID (), (int)neighbor->id, (int)drift);
//#endif
//                }
//            }
//        }
//    } // end looping through all other nodes


//}

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
}
