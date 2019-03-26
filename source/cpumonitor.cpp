#include "cpumonitor.h"
#include <QDebug>
cpuMonitor::cpuMonitor():
    cpu1Series(0),cpu1History(120,0),pen1(Qt::yellow),
    cpu2Series(0),cpu2History(120,0),pen2(Qt::red)
{
    cpu1Series = new QSplineSeries(this);
    cpu2Series = new QSplineSeries(this);
}

cpuMonitor::~cpuMonitor(){
    delete cpu1Series;
    delete cpu2Series;
}

void cpuMonitor::run(){
    return;
}

void cpuMonitor::updateCpu(){
    preidle1 = idle1;   pretotal1 = total1;
    preidle2 = idle2;   pretotal2 = total2;

    int cpustat1[10];
    int cpustat2[10];

    int i;
    char line[256];
    fp = fopen("/proc/stat","r");
    fgets(line,sizeof(line),fp);
    /*cpu1*/
    fgets(line,sizeof(line),fp);
    sscanf(line,"%*s%d%d%d%d%d%d%d%d%d%d",&cpustat1[0],&cpustat1[1],\
            &cpustat1[2],&cpustat1[3],&cpustat1[4],&cpustat1[5],\
            &cpustat1[6],&cpustat1[7],&cpustat1[8],&cpustat1[9]);
    /*cpu2*/
    fgets(line,sizeof(line),fp);
    sscanf(line,"%*s%d%d%d%d%d%d%d%d%d%d",&cpustat2[0],&cpustat2[1],\
            &cpustat2[2],&cpustat2[3],&cpustat2[4],&cpustat2[5],\
            &cpustat2[6],&cpustat2[7],&cpustat2[8],&cpustat2[9]);

    for(i=0,total1=0,total2=0;i<10;++i){
        total1 += cpustat1[i];
        total2 += cpustat2[i];
    }
    idle1 = cpustat1[3];
    idle2 = cpustat2[3];

    cpu1History.pop_front();
    cpu1History.push_back(100-((idle1-preidle1)*100/(total1-pretotal1)));
    cpu2History.pop_front();
    cpu2History.push_back(100-((idle2-preidle2)*100/(total2-pretotal2)));
    cpu1Series->clear();    cpu2Series->clear();
    cpu1Series->setPen(pen1);   cpu2Series->setPen(pen2);
    for(i=0;i<120;++i){
        cpu1Series->append(i-120,cpu1History.at(i));
        cpu2Series->append(i-120,cpu2History.at(i));
    }


}
