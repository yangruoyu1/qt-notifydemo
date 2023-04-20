#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include <QObject>

class notifymanager : public QObject
{
    Q_OBJECT
public:
    explicit notifymanager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // NOTIFYMANAGER_H