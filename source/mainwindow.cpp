#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("System Monitor");

    timer = new QTimer(this);
    currentTime = new QLabel(this);
    ui->statusBar->addWidget(currentTime);
    memChart = new QChart();
    cpuChart = new QChart();
    memMnt = new memMonitor(Qt::red,Qt::blue);
    cpuMnt = new cpuMonitor();

    memMnt->start();
    cpuMnt->start();
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    initTab();


    memChart->addSeries(memMnt->memSeries);
    memChart->addSeries(memMnt->swapSeries);
    memChart->legend()->hide();
    memChart->createDefaultAxes();
    memChart->axisX()->setRange(-120,0);
    memChart->axisY()->setRange(0,100);


    cpuChart->addSeries(cpuMnt->cpu1Series);
    cpuChart->addSeries(cpuMnt->cpu2Series);
    cpuChart->legend()->hide();
    cpuChart->createDefaultAxes();
    cpuChart->axisX()->setRange(-120,0);
    cpuChart->axisY()->setRange(0,100);

    connect(timer,SIGNAL(timeout()),this,SLOT(updateTab()));
    connect(timer,SIGNAL(timeout()),memMnt,SLOT(updateMem()));
    connect(timer,SIGNAL(timeout()),cpuMnt,SLOT(updateCpu()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
    connect(ui->tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(newSelectedItem(int,int)));
    timer->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete memChart;
    delete memMnt;
    delete cpuChart;
    delete cpuMnt;
}

void MainWindow::initTab(){
    ui->Memtotal->setText(QString::number(memMnt->totalMem));
    ui->Swaptotal->setText(QString::number(memMnt->totalSwap));
    ui->memBar->setRange(0,memMnt->totalMem);
    ui->swapBar->setRange(0,memMnt->totalSwap);
    ui->cpu1Bar->setRange(0,100);
    ui->cpu2Bar->setRange(0,100);
    ui->red->setStyleSheet("QLabel { background-color : red;}");
    ui->yellow->setStyleSheet("QLabel{background-color : yellow;}");

    ui->memChart->setChart(memChart);
    ui->memChart->setRenderHint(QPainter::Antialiasing);

    ui->cpuChart->setChart(cpuChart);
    ui->cpuChart->setRenderHint(QPainter::Antialiasing);

    int fd1,fd2;
    char buf1[32],buf2[32];
    int len;
    /*** hostname ***/
    fd1 = open("/proc/sys/kernel/hostname",O_RDONLY);
    if(fd1 == -1){
        //QMessageBox::warning();
    }
    len = read(fd1,buf1,sizeof(buf1));
    buf1[len-1] = '\0';
    ui->hostname->setText(QString::fromLocal8Bit(buf1));
    ::close(fd1);

    /*** os version(type+release) ***/
    fd1 = open("/proc/sys/kernel/ostype",O_RDONLY);
    fd2 = open("/proc/sys/kernel/osrelease",O_RDONLY);
    if(fd1 == -1 || fd2 == -1){
        //QMessageBox::warning();
    }

    len = read(fd1,buf1,sizeof(buf1));
    buf1[len-1] = '\0';
    len = read(fd2,buf2,sizeof(buf2));
    buf2[len-1] = '\0';
    ui->Version->setText(QString::fromLocal8Bit(buf1) \
                  + "-" +QString::fromLocal8Bit(buf2));
    ::close(fd1);::close(fd2);

    /*** start time ***/
    time_t t;
    double dualtime;
    char *starttime;
    fd1 = open("/proc/uptime",O_RDONLY);
    if(fd1 == -1){
        //QMessageBox::warning();
    }
    read(fd1,buf1,sizeof(buf1));
    sscanf(buf1,"%lf",&dualtime);
    time(&t);
    t -= (int)dualtime;
    starttime = ctime(&t);
    QString tempStr = QString::fromLocal8Bit(starttime);
    tempStr.chop(1);
    ui->StartTime->setText(tempStr);
    ::close(fd1);

    /***cpu info***/
    FILE *fp;
    int i;
    char line[128],*begin;
    fp = fopen("/proc/cpuinfo","r");
    if(fp == NULL){
        //QMessageBox::warning();
    }
    while(fgets(line,sizeof(line),fp)){
        if(strstr(line,"model name")){
            for(i=0;line[i] != ':';++i);
            begin = line + i + 2;
            ui->cpuList->insertPlainText(QString::fromLocal8Bit(begin));
        }
    }
}

void MainWindow::updateTab(){
    char buf[64];
    time_t t;
    time(&t);
    currentTime->setText("now : " + QString::fromLocal8Bit(ctime(&t)));
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        ui->Memuse->setText(QString::number(memMnt->memHistory.back()));
        ui->SwapUse->setText(QString::number(memMnt->swapHistory.back()));
        ui->Memfree->setText(QString::number(memMnt->freeMem));
        ui->Swapfree->setText(QString::number(memMnt->freeSwap));
        ui->memBar->setValue(memMnt->memHistory.back());
        ui->swapBar->setValue(memMnt->swapHistory.back());
        break;
    case 1:
        ui->cpu1Bar->setValue(cpuMnt->cpu1History.back());
        ui->cpu2Bar->setValue(cpuMnt->cpu2History.back());
        break;
    case 2:
        updatePro();
        break;
    case 3:
        int fd;
        double dualtime;
        fd = open("/proc/uptime",O_RDONLY);
        read(fd,buf,sizeof(buf));
        sscanf(buf,"%lf",&dualtime);
        ui->DualTime->setText(QString::number(dualtime)+"s");
        break;
    default:
        break;
    }

}

void MainWindow::tabChanged(int index){
    if(index == 2) updateTab();
}

void MainWindow::updatePro(){
        QDir qd("/proc");
        QStringList qsList = qd.entryList();
        qsList.pop_front();
        qsList.pop_front();
        //将.和..目录排除
        QString qs,tempStr,procName,state;
        FILE *fp;
        char line[256];
        int i = 0;
        int nR = 0, nS = 0, nT = 0, nI = 0, nZ = 0;
        int pid;
        double Memory;
        //处理每个代表进程的目录
        while(!qsList.isEmpty()){
            qs = qsList.first();
            qsList.pop_front();
            QFile tempFile;
            QString filename = "/proc/" + qs + "/stat";
            tempFile.setFileName(filename);
            filename += "us";//[pid]/status
            fp = fopen(filename.toLocal8Bit(),"r");
            if(!tempFile.open(QIODevice::ReadOnly) || NULL == fp){
                continue;
            }
            tempStr = tempFile.readLine();
            if(tempStr.length() == 0)continue;
            QTableWidgetItem *item = new QTableWidgetItem();
            pid = tempStr.section(' ',0,0).toInt();
            item->setData(Qt::DisplayRole,pid);
            ui->tableWidget->setItem(i,0,item);//pid

            procName = tempStr.section(' ',1,1);
            item = new QTableWidgetItem(procName.mid(1,procName.length()-2));
            ui->tableWidget->setItem(i,1,item);//name

            state = tempStr.section(' ',2,2);
            item = new QTableWidgetItem(state);
            ui->tableWidget->setItem(i,2,item);//state            
//            /* states in TASK_REPORT: */
//            "R (running)",		/* 0x00 */
//            "S (sleeping)",		/* 0x01 */
//            "D (disk sleep)",	/* 0x02 */
//            "T (stopped)",		/* 0x04 */
//            "t (tracing stop)",	/* 0x08 */
//            "X (dead)",		/* 0x10 */
//            "Z (zombie)",		/* 0x20 */
//            "P (parked)",		/* 0x40 */

//            /* states beyond TASK_REPORT: */
//            "I (idle)",		/* 0x80 */
            switch(state.at(0).toLatin1()){
                case 'R':
                    ++nR;break;
                case 'S':
                    ++nS;break;
                case 'T':
                    ++nT;break;
                case 'I':
                    ++nI;break;
                case 'Z':
                    ++nZ;break;
                default:
                    break;
            }
            ui->label_Running->setText(QString::number(nR));
            ui->label_Zombie->setText(QString::number(nZ));
            ui->label_Sleeping->setText(QString::number(nS));
            ui->label_Stopped->setText(QString::number(nT));
            ui->label_Idle->setText(QString::number(nI));
            item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole,tempStr.section(' ',3,3).toInt());
            ui->tableWidget->setItem(i,3,item);//ppid

            item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole,tempStr.section(' ',17,17).toInt());
            ui->tableWidget->setItem(i,4,item);//priority

            Memory = 0;
            while(fgets(line,sizeof(line),fp)){
              if(strstr(line,"RssAnon") != NULL){
                  sscanf(line,"%*s%lf",&Memory);
              }
            }
            item = new QTableWidgetItem();
            if(Memory == 0){
                item->setText(" ");
            }else{
                item->setData(Qt::DisplayRole,Memory/1024);
            }
            ui->tableWidget->setItem(i,5,item);

            tempFile.close();
            fclose(fp);
            ++i;
        }
        ui->tableWidget->setRowCount(i);
        ui->label_Total->setText(QString::number(i));
        return;
}

void MainWindow::newSelectedItem(int row, int culomn){
    cRow = row;cCulomn = culomn;
}

void MainWindow::on_btn_viewProc_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row != -1){
        QFile tmpFile("/proc/"+ui->tableWidget->item(row,0)->text()+"/status");
        tmpFile.open(QIODevice::ReadOnly);
        QMessageBox::information(this,"process information",tmpFile.readAll(),QMessageBox::Yes);
    }
    //qDebug()<<ui->tableWidget->itemAt(ui->tableWidget->currentRow(),1)->data(Qt::DisplayRole);
}

void MainWindow::on_btn_killProc_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row != -1){
        int rt = QMessageBox::warning(this,ui->tableWidget->item(row,1)->text(),"Are you sure to kill the selected process?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
        if(rt == QMessageBox::Yes)
        system("kill " + ui->tableWidget->item(row,0)->text().toLocal8Bit());
    }

}

void MainWindow::on_btn_ShutDown_clicked()
{
    system("shutdown -h now");
}

void MainWindow::on_pushButton_clicked()
{
    QString key = QInputDialog::getText(this,"查找进程","pid\\name:");
    auto list= ui->tableWidget->findItems(key,Qt::MatchStartsWith);

    while(!list.isEmpty() && list.front()->column() > 2){
        list.pop_front();
    }

    if(list.isEmpty()){
        QMessageBox::warning(this,"查找进程"," 没有该进程",QMessageBox::Ok);
    }
    else {
        ui->tableWidget->setCurrentCell(list.front()->row(),0);
    }

}
