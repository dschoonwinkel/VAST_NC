#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QtGui>
#include "VASTsim.h"
#include "VAST/vaststatlog.h"
#include <fstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void calc_consistency (const Vast::VASTStatLog &restoredLog, size_t &total_AN_actual,
                           size_t &total_AN_visible, size_t &total_drift, size_t &max_drift,
                           size_t &drift_nodes, timestamp_t latest_timestamp);

protected:
    void paintEvent(QPaintEvent*) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:

    void nextTimestep();
    void calculateUpdate();
    void outputResults();

    Ui::MainWindow *ui;
    int m_timerId;

    SimPara simpara;
    VASTPara_Net netpara;
    map <int, vector<Node *> *> nodes;

    //# nodes currently created
    int nodes_created = 0;
    int create_countdown = 0;

    //Simulation moves
//    size_t steps = 0;
    bool running = true;

    void setUpColors();
    vector<QColor> nodeColors;
    std::map<Vast::id_t, VASTStatLog> allRestoredLogs;
    std::vector<Vast::id_t> logIDs;
    timestamp_t latest_timestamp;

    std::string results_file = "./logs/results/results1.txt";
    std::ofstream ofs;

    bool paused = false;

    #define DIM_X 768
    #define DIM_Y 768

    QPoint lastMouseClickPoint;
    Vast::id_t activeNode = -1;

    //Variables needed for calc_consistency
    size_t total_AN_actual =0, total_AN_visible =0, total_drift =0, max_drift =0, drift_nodes =0;
    size_t worldSendStat =0, worldRecvStat = 0;


    size_t total_active_nodes =0;

};

#endif // MAINWINDOW_H
