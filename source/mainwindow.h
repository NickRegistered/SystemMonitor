#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QMessageBox>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "memmonitor.h"
#include "cpumonitor.h"
#include <QChartView>
#include <QInputDialog>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTimer *timer;

private:
    Ui::MainWindow *ui;
    memMonitor *memMnt;
    QChart *memChart;
    cpuMonitor *cpuMnt;
    QChart *cpuChart;
    QLabel *currentTime;
    int cRow,cCulomn;
    void updatePro();
private slots:
    void initTab();
    void updateTab();
    void tabChanged(int);
    void on_btn_viewProc_clicked();
    void on_btn_killProc_clicked();
    void on_btn_ShutDown_clicked();
    void newSelectedItem(int,int);
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
