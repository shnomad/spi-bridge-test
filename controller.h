#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>
#include "common.h"
#include "coding_channel.h"

class coding_channel;

class Controller : public QObject
{
    Q_OBJECT

public:

    explicit Controller(QObject *parent = nullptr);
    ~Controller();

signals:


public slots:

private:
    coding_channel *m_ch[15];
    QThread *m_pthread[15];
    quint8 max_thread_count = 1;
};

#endif // CONTROLLER_H
