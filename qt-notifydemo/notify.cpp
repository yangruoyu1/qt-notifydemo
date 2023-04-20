#include "notify.h"

#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QPropertyAnimation>
#include<QTimer>
#include<QMouseEvent>
#include<QDesktopServices>

Notify::Notify(int displayTime,QWidget *parent) : QWidget(parent),
    displayTime(displayTime)
{
    //设置窗口属性为无边框|显示系统菜单|新窗口为工具类窗口|总保持在最上面
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint|Qt::Tool|Qt::WindowStaysOnTopHint);
    //窗口部件背景透明化
    this->setAttribute(Qt::WA_NoSystemBackground,true);
     //窗口部件背景透明化
    this->setAttribute(Qt::WA_TranslucentBackground,true);

    //设置背景标签
    backgroundLabel=new QLabel(this);
    backgroundLabel->move(0,0);
    backgroundLabel->setObjectName("notify-background");

    //设置图片标签，基于背景标签
    iconLabel=new QLabel(backgroundLabel);
    iconLabel->setFixedWidth(40);
    //消除布局中的空隙，让两个控件紧挨在一起
    iconLabel->setAlignment(Qt::AlignCenter);

    //设置标题标签，也是基于背景标签
    titleLabel=new QLabel(backgroundLabel);
    titleLabel->setObjectName("notify-title");

    //设置主体标签，也是基于背景标签
    bodyLabel=new QLabel(backgroundLabel);
    bodyLabel->setObjectName("notify-body");

    //获得主体标签的字体
    QFont font=bodyLabel->font();
    //设置字体高度占用的像素px大小（setPointSize表示占用的可屋里测量的长度）
    font.setPixelSize(12);
    bodyLabel->setFont(font);

    //设置垂直布局，加入标题标签和主题标签
    QVBoxLayout *contentLayout=new QVBoxLayout();
    contentLayout->addWidget(titleLabel);
    contentLayout->addWidget(bodyLabel);

    //设置水平布局,加入图片标签
    QHBoxLayout *mainLayout=new QHBoxLayout(backgroundLabel);
    mainLayout->addWidget(iconLabel);
    //addSpacing是在layout中的控件之间插入间距，其插入的间距是在setSpacing(int)基础上的
    mainLayout->addSpacing(5);
    mainLayout->addLayout(contentLayout);

    closeBtn=new QPushButton("x",backgroundLabel);
    closeBtn->setObjectName("notify-close-btn");
    closeBtn->setFixedSize(24,24);
    connect(closeBtn,&QPushButton::clicked,this,[this]{
        Q_EMIT disappeared();
    });
//设置图形界面的外观
    this->setStyleSheet("#notify-background{"
                        "border:1px solid #ccc;"
                        "background:white;"
                        "border-radius:4px;"
                        "}"
                        "#notify-title{"
                        "font-weight:bold;"
                        "color:#333;"
                        "font-size:14px;"
                        "}"
                        "#notify-body{"
                        "color:#444;"
                        "}"
                        "#notify-close-btn{"
                        "border:0;"
                        "color:#999;"
                        "}"
                        "#notify-close-btn:hover{"
                        "background:#ccc;"
                        "}");
}

void Notify::showGriant()
{
    this->show();
    titleLabel->setText(title);
    //使用QPixmap是在屏幕上显示图像并进行设计和优化而使用的
    QPixmap tempPix=QPixmap(this->icon);
    //将对象缩放为大小为30*30像素的图像，同时保持其宽度高度不变
    tempPix=tempPix.scaled(QSize(30,30),Qt::KeepAspectRatio);
    iconLabel->setPixmap(tempPix);

    backgroundLabel->setFixedSize(this->size());
    closeBtn->move(backgroundLabel->width()-closeBtn->width(),0);

    //超过长度省略号
    /*该对象使用了一个名为bodyLabel的QLabel对象的字体。
     QFontMetrics类提供了一些方便的方法来测量和处理字体文本的大小和形状。
    使用QFontMetrics对象的elidedText()方法来处理一个QString类型的文本，即this->body。
    这个方法的作用是将文本截断为适合在给定宽度的空间内显示的最大长度，并在末尾添加省略号。
    其中，第二个参数Qt::ElideRight指定省略号将出现在文本的右侧，即省略号将在文本的结尾处出现。
    第三行代码将处理后的文本设置为bodyLabel对象的文本，以便在用户界面中显示。
    其中，文本的宽度被减去了5，这是因为我们需要留出一些空间以避免文本显示超出QLabel对象的边界。*/
    QFontMetrics elidfont(bodyLabel->font());
    QString text=elidfont.elidedText(this->body,Qt::ElideRight,bodyLabel->width()-5);
    bodyLabel->setText(text);

/*QPropertyAnimation类来创建一个窗口透明度动画，使窗口从完全透明变为完全不透明。
其中第一个参数"windowOpacity"指定了要进行动画的属性名称，即窗口的透明度属性。
第二个参数this指定了要对哪个对象的属性进行动画，这里是当前对象。
第三个参数this指定了QPropertyAnimation对象的父对象，这里也是当前对象。
setStartValue()方法将动画的起始值设置为0，即窗口完全透明。
setEndValue()方法将动画的结束值设置为1，即窗口完全不透明。
setDuration()方法将动画的持续时间设置为200毫秒。
start()方法启动动画。一旦动画开始，窗口的透明度属性将从起始值0逐渐变化到结束值1，持续时间为200毫秒。
这将导致窗口的外观逐渐从完全透明变为完全不透明，从而实现了淡入效果。*/
    QPropertyAnimation *animation=new QPropertyAnimation(this,"windowOpacity",this);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setDuration(200);
    animation->start();

/*第一行代码使用connect()方法将QPropertyAnimation对象的finished()信号连接到一个lambda函数上。
当动画完成时，finished()信号将触发lambda函数执行。
lambda函数中的第一行代码使用deleteLater()方法删除QPropertyAnimation对象。
这是为了避免动画结束后占用内存。注意，此处使用了动态分配的QPropertyAnimation对象，因此必须在使用完毕后手动删除。
lambda函数中的第二行代码使用QTimer::singleShot()方法，该方法将在指定的时间间隔后执行一个lambda函数。
这里的时间间隔是displayTime，表示动画结束后多长时间执行该lambda函数。
最后一行代码调用了一个名为hideGriant()的成员函数，它将当前对象隐藏。
根据代码上下文，这个函数可能是将一个渐变窗口隐藏的函数。*/
    connect(animation,&QPropertyAnimation::finished,this,[animation,this](){
        animation->deleteLater();
        QTimer::singleShot(displayTime,this,[this](){
            this->hideGriant();
        });
    });
}

//从当前窗口透明度变为完全透明
void Notify::hideGriant()
{
    //第一个参数"windowsOpacity"指定了要进行动画的属性名称，即窗口的透明度属性。
    //第二个参数this指定了要对哪个对象的属性进行动画，这里是当前对象。
    //第三个参数this指定了QPropertyAnimation对象的父对象，这里也是当前对象。
    QPropertyAnimation *animation=new QPropertyAnimation(this,"windowsOpacity",this);

    animation->setStartValue(this->windowOpacity());
    animation->setEndValue(0);
    animation->setDuration(200);
    animation->start();

    connect(animation,&QPropertyAnimation::finished,this,[animation,this](){
/*lambda函数中的第一行代码调用hide()函数将当前对象隐藏。
deleteLater()方法删除QPropertyAnimation对象。
使用Q_EMIT发射一个自定义的信号disappeared()。
这个信号可能是用来告知其他对象当前对象已经被隐藏了。
注意，这里使用了新的C++11信号槽语法，Q_EMIT是一个宏，用于发射信号。*/
        this->hide();
        animation->deleteLater();
        Q_EMIT disappeared();
    });
}

void Notify::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(!url.isEmpty()){
            QDesktopServices::openUrl(url);
        }
        hideGriant();
    }
}

void Notify::setUrl(const QString &value)
{
    url=value;
}

void Notify::setBody(const QString &value)
{
    body=value;
}

void Notify::setTitle(const QString &value)
{
    title=value;
}

void Notify::setIcon(const QString &value)
{
    icon=value;
}
