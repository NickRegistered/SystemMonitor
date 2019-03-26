#ifndef MEMMONITOR_H
#define MEMMONITOR_H

#include <QThread>
#include <QSplineSeries>
#include <deque>
using namespace std;
using namespace QtCharts;
class memMonitor:public QThread
{
    Q_OBJECT
protected:
    void run();
public:
    memMonitor(Qt::GlobalColor memColor,Qt::GlobalColor swapColor);
    int totalMem;
    int totalSwap;
    int freeMem;
    int freeSwap;
    QPen memPen;
    QPen swapPen;
    deque<int> memHistory;
    deque<int> swapHistory;
    QSplineSeries *memSeries;
    QSplineSeries *swapSeries;
    ~memMonitor();
private:
    FILE *fp;
private slots:
    void updateMem();
};

#endif // MEMMONITOR_H
