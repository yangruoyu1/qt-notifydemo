#include "notifymanager.h"
#include<QDesktopWidget>
#include<QApplication>
#include<QDebug>
#include<QTimer>
#include<QPropertyAnimation>

const int RIGHT=10;
const int BOTTOM=10;
const int HEIGHT=60;
const int WIDTH=300;
const int SPACE=20;

/*NotifyManager类继承自QObject。QObject的构造函数可以接受一个QObject对象作为参数，
用于指定新对象的父对象。构造函数的参数parent被传递给了QObject的构造函数，
表示NotifyManager对象的父对象为parent。*/
notifymanager::notifymanager(QObject *parent) :QObject(parent),
    maxCount(5),
    displayTime(10*500)
{

}

/*notify()函数的第一行代码将一个NotifyData对象（包含图标、标题、正文和相关URL）
添加到dataQueue队列的末尾，表示将该通知数据添加到了待显示的队列中。
notify()函数的第二行代码调用showNext()函数，用于判断是否可以显示下一条通知，
并将待显示的通知数据传递给Notify类的对象进行显示。*/
void notifymanager::notify(const QString &title, const QString &body, const QString &icon, const QString url)
{
    dataQueue.enqueue(NotifyData(icon,title,body,url));
    showNext();
}

void notifymanager::setMaxCount(int count)
{
    maxCount=count;
}

void notifymanager::setDisPlayTime(int ms)
{
    displayTime=ms;
}

/* 用于重新排列当前显示的通知，保证它们不会重叠。*/
void notifymanager::rearrange()
{
    /*这段代码获取当前桌面的可用区域（不包括任务栏等其他窗口）并计算出屏幕右下角的坐标。
首先，调用QApplication类的静态函数desktop()获取当前桌面的QDesktopWidget对象。
然后，调用QDesktopWidget对象的availableGeometry()函数获取桌面的可用区域，
并将其赋值给desktopRect变量。availableGeometry()函数返回一个QRect对象，
表示可用区域的左上角坐标和宽度、高度。
最后，计算出屏幕右下角的坐标bottomRight，即desktopRect的右下角坐标。
此坐标用于重新排列通知的位置，确保它们不会超出屏幕范围。*/
    QDesktopWidget *desktop=QApplication::desktop();
    QRect desktopRect=desktop->availableGeometry();
    QPoint bottomRight=desktopRect.bottomRight();

    QList<Notify*>::iterator i;
    for(i=notifyList.begin();i!=notifyList.end();i++)
    {
/*这段代码重新排列通知窗口的位置。
首先，通过notifyList.indexOf((*i))获取当前迭代器指向的元素在通知列表中的索引index。
然后，计算通知窗口的新位置pos，通过屏幕右下角坐标bottomRight、通知窗口的宽度和高度（WIDTH、HEIGHT）
以及相邻通知窗口之间的间距（SPACE）计算出通知窗口左上角的坐标。
其中，BOTTOM为通知窗口底部到屏幕底部的距离。
接下来，创建一个QPropertyAnimation对象animation，将其绑定到当前迭代器指向的通知窗口(*i)的位置属性上。
这里的位置属性是通过QWidget类继承得到的，即QWidget::pos()。
QPropertyAnimation的作用是控制QWidget的位置属性从当前值逐渐变化到指定的目标值pos。
设置animation的起始值为当前窗口的位置，即(*i)->pos()，目标值为新位置pos。动画持续时间为300毫秒。
最后，调用animation的start()函数启动动画，并将其与一个lambda函数连接，
当动画结束时，删除animation对象*/
        int index=notifyList.indexOf(*i);
        QPoint pos=bottomRight-QPoint(WIDTH+RIGHT,(HEIGHT+SPACE)*(index+1)-SPACE+BOTTOM);
        QPropertyAnimation *animation=new QPropertyAnimation((*i),"pos",this);
        animation->setStartValue((*i)->pos());
        animation->setEndValue(pos);
        animation->setDuration(300);
        animation->start();

        connect(animation,&QPropertyAnimation::finished,this,[animation,this](){
            animation->deleteLater();
        });
    }
}

//主要函数，显示提示框
void notifymanager::showNext()
{
    if(notifyList.size()>=maxCount||dataQueue.isEmpty()){
        return;
    }

    NotifyData data=dataQueue.dequeue();
    Notify *notify=new Notify(this->displayTime);
    notify->setIcon(data.icon);
    notify->setTitle(data.title);
    notify->setBody(data.body);
    notify->setUrl(data.url);
    notify->setFixedSize(WIDTH,HEIGHT);

    QDesktopWidget *desktop=QApplication::desktop();
    QRect desktopRect=desktop->availableGeometry();
    QPoint bottomRight=desktopRect.bottomRight();
    QPoint pos=bottomRight-QPoint(notify->width()+RIGHT,(HEIGHT+SPACE)*(notifyList.size()+1)-SPACE+BOTTOM);

    /*将新的通知添加到通知队列中，并显示通知界面。
首先，使用之前获取的底部右侧点和通知窗口的宽度、高度以及间隙和底部距离，计算出新通知窗口的左上角位置pos。
接下来，将新通知移动到计算出的位置并显示通知。然后，将新通知添加到通知列表中。
最后，将新通知的disappeared信号连接到lambda表达式中，以便在通知界面关闭时从列表中移除该通知，
然后重排列表，如果通知列表数量达到最大值，那么在短暂延迟后，继续显示下一个通知。
最后，使用deleteLater()在稍后删除该通知窗口对象。*/
    notify->move(pos);
    notify->showGriant();
    notifyList.append(notify);

    connect(notify,&Notify::disappeared,this,[notify,this](){
       this->notifyList.removeAll(notify);
       this->rearrange();

        if(this->notifyList.size()==this->maxCount-1)
        {
            QTimer::singleShot(300,this,[this]{
                this->showNext();
            });
        }else{
            this->showNext();
        }
        notify->deleteLater();
    });
}
