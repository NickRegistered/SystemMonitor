# SystemMonitor
采用QT图形界面编程，通过/proc文件监测linux系统各项情况

实验指导见课程设计_2019.ppt文件

关于QT中使用控件显示曲线：

&emsp;在工程文件中包含QChart库，使用QGraphicsView控件，将类型提升为QChartsView。

&emsp;将需要显示的点加入到QSplineSeries中，将需要显示的曲线加入到QChartsView控件即可实现曲线显示。

运行效果预览：

内存情况监测：

![内存使用](https://github.com/NickRegistered/SystemMonitor/blob/master/pictures/%E5%86%85%E5%AD%98%E4%BD%BF%E7%94%A8%E6%83%85%E5%86%B5.png);

CPU情况监测：

![CPU使用](https://github.com/NickRegistered/SystemMonitor/blob/master/pictures/CPU%E4%BD%BF%E7%94%A8%E6%83%85%E5%86%B5.png);

进程管理：

![进程管理](https://github.com/NickRegistered/SystemMonitor/blob/master/pictures/%E8%BF%9B%E7%A8%8B%E7%AE%A1%E7%90%86.png);

系统信息：

![系统信息](https://github.com/NickRegistered/SystemMonitor/blob/master/pictures/%E7%B3%BB%E7%BB%9F%E4%BF%A1%E6%81%AF.png);
