#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //explicit指定构造函数为显示，阻止构造函数发生隐式转换和复制初始化
    explicit MainWindow(QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:
};

#endif // MAINWINDOW_H
