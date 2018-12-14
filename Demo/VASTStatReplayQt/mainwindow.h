#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QtGui>
#include "VASTVerse.h"
#include "vaststatlog.h"
#include "replay_functions.h"
#include <fstream>

using namespace Vast;

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
    void paintEvent(QPaintEvent*) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:

    void nextTimestep();

    Ui::MainWindow *ui;
    int m_timerId;

    //# nodes currently created
    int nodes_created = 0;
    int create_countdown = 0;

    //Simulation moves
//    size_t steps = 0;
    bool running = true;

    void setUpColors();
    vector<QColor> nodeColors;
//    std::map<Vast::id_t, Vast::VASTStatLog> allRestoredLogs;


    bool paused = false;
    bool finished = false;

    #define DIM_X 768
    #define DIM_Y 768

    QPoint lastMouseClickPoint;
    Vast::id_t activeNode = -1;

};

#endif // MAINWINDOW_H
