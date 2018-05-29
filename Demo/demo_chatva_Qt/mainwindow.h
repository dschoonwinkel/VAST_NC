#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QLineEdit>
#include <QTextEdit>
#include "VASTsim.h"
#include "ui_mainwindow.h"

#define TIMER_INTERVAL		50                  // interval in ms for screen & input refresh
#define UPDATE_INTERVAL     100                 // interval in ms for sending movement updates
#define INPUT_SIZE          200                 // size for chat message

#define VAST_EVENT_LAYER    1                   // layer ID for sending events
#define VAST_UPDATE_LAYER   2                   // layer ID for sending updates

#define SIZE_TEXTAREA       4

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    int m_timerId;

    void nextTimestep();
    void MoveOnce();
    void CheckJoin();
    bool StoreChat (std::string &msg);

    //VAST variables
    VASTVerse * g_world = NULL;
    VAST * g_self = NULL;
    Vast::id_t g_sub_no = 0;

    int g_node_no = -1;
    Area g_aoi;
    world_t g_world_id = 0;
    Vast::NodeState g_state = ABSENT;
    Voronoi * g_Voronoi = NULL;
    VASTPara_Net g_netpara;
    SimPara g_simpara;
    bool is_gateway;
    int g_steps = 0;

    //GUI settings
    int NODE_RADIUS = 10;
    bool follow_mode = false;
    bool show_edges = true;
    char last_char = 0;
    bool finished = false;      //Simulation is done
    QPointF g_origin;       //Viewport origin position

    vector<string> g_chatmsg;
    vector<IPaddr> entries;
    Addr g_gateway;         //Address to gateway

    void setUpColors();
    vector<QColor> nodeColors;

//    QLineEdit* chatInput;

private slots:
    void returnPressed();
};

#endif // MAINWINDOW_H
