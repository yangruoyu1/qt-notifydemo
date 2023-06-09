#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include <QObject>
#include<QQueue>
#include"notify.h"

class notifymanager : public QObject
{
    Q_OBJECT
public:
    explicit notifymanager(QObject *parent = nullptr);

    void notify(const QString &title,const QString &body,const QString &icon,const QString url);
    void setMaxCount(int count);
    void setDisPlayTime(int ms);

private:
    class NotifyData{
    public:
        NotifyData(const QString &icon,const QString &title,const QString &body,const QString &url):
        icon(icon),
        title(title),
        body(body),
        url(url)
        {}

        QString icon;
        QString title;
        QString body;
        QString url;
    };

    void rearrange();
    void showNext();

    QQueue<NotifyData>dataQueue;
    QList<Notify*> notifyList;

    int maxCount;
    int displayTime;



};

#endif // NOTIFYMANAGER_H
