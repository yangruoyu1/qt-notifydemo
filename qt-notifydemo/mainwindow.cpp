#include "mainwindow.h"
#include "notifymanager.h"
#include<QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QPushButton *button=new QPushButton("弹窗",this);
    notifymanager *manager=new notifymanager(this);

    connect(button,&QPushButton::clicked,manager,[manager]{
        manager->notify("新消息","有一个新的消息","/image/message.png","https://github.com/yangruoyu1/qt-notifydemo/tree/main/qt-notifydemo");
    });
}


