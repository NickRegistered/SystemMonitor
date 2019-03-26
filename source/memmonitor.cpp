#include "memmonitor.h"
#include <QDebug>
memMonitor::memMonitor(Qt::GlobalColor memColor,Qt::GlobalColor swapColor):
    memPen(memColor),swapPen(swapColor),
    memHistory(120,0),swapHistory(120,0),memSeries(0),swapSeries(0)
{
    memSeries = new QSplineSeries(this);
    swapSeries = new QSplineSeries(this);
    int i = 0;
    char line[256];
    fp = fopen("/proc/meminfo","r");
    while(fgets(line,sizeof(line),fp) && i != 16){
        switch (i) {
        case 0:
            sscanf(line,"%*s%d",&totalMem);
            break;
        case 14:
            sscanf(line,"%*s%d",&totalSwap);
        default:
            break;
        }
        ++i;
    }
    fclose(fp);
}

void memMonitor::run(){
    return;
}

memMonitor::~memMonitor(){
    delete memSeries;
    delete swapSeries;
}

void memMonitor::updateMem(){
    fp = fopen("/proc/meminfo","r");
    int i = 0;
    char line[256];
    int usedMem,usedSwap;
    while(fgets(line,sizeof(line),fp) && i!=16){
        switch (i) {
        case 2:
            sscanf(line,"%*s%d",&freeMem);
            break;
        case 15:
            sscanf(line,"%*s%d",&freeSwap);
        default:
            break;
        }
        ++i;
    }
    fclose(fp);

    usedMem = totalMem - freeMem;
    usedSwap = totalSwap - freeSwap;
    memHistory.pop_front();
    memHistory.push_back(usedMem);
    memSeries->clear();
    memSeries->setPen(memPen);
    for(i=0;i<120;++i){
        memSeries->append(i-120,memHistory.at(i)*100/totalMem);
    }

    swapHistory.pop_front();
    swapHistory.push_back(usedSwap);
    swapSeries->clear();
    swapSeries->setPen(swapPen);
    for(i=0;i<120;++i){
        swapSeries->append(i-120,swapHistory.at(i)*100/totalSwap);
    }

    return;
}
