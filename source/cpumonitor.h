#ifndef CPUMONITOR_H
#define CPUMONITOR_H
#include <QThread>
#include <QSplineSeries>
#include <QTimer>
#include <deque>
using namespace std;
using namespace QtCharts;
class cpuMonitor:public QThread
{
    Q_OBJECT
protected:
    void run();
public:
    cpuMonitor();
    QPen pen1;
    QPen pen2;
    QSplineSeries *cpu1Series;
    deque<int> cpu1History;
    QSplineSeries *cpu2Series;
    deque<int> cpu2History;
    ~cpuMonitor();
private:
    FILE *fp;
    int total1,pretotal1;
    int total2,pretotal2;
    int idle1,preidle1;
    int idle2,preidle2;
private slots:
    void updateCpu();
};

#endif // CPUMONITOR_H
