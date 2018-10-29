#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "VASTsim.h"
#include "VAST/vaststatlog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void calc_consistency (size_t i, size_t &AN_actual, size_t &AN_visible, size_t &total_drift, size_t &max_drift, size_t &drift_nodes);

protected:
    void paintEvent(QPaintEvent*) override;
    void timerEvent(QTimerEvent *event) override;

private:

    void nextTimestep();


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
//    std::vector<Vast::VASTStatLog> restoredLogs;
    std::map<Vast::id_t, std::vector<VASTStatLog>> allRestoredLogs;
    std::vector<Vast::id_t> logIDs;
    std::vector<size_t> log_steps;
    timestamp_t latest_timestamp;

    #define DIM_X 768
    #define DIM_Y 768

};

#endif // MAINWINDOW_H
