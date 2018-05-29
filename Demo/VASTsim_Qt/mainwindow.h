#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "VASTsim.h"

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
    int steps = 0;
    bool running = true;

    void setUpColors();
    vector<QColor> nodeColors;

};

#endif // MAINWINDOW_H
