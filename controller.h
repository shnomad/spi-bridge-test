#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QSignalMapper>
#include "common.h"

class CodingChannel;
class GpioControl;
class client;

class Controller : public QObject
{
    Q_OBJECT

public:

    explicit Controller(QObject *parent = nullptr);
    void thread_comm();
    ~Controller();

signals:

public slots:

private:

    QSignalMapper *signalMapper;

    CodingChannel *m_ch[15];
    QThread *m_coding_pThread[15], *m_sock_pThread;
    quint8 max_thread_count=1;

    GpioControl *m_GpioControl;
    QTimer gpio_delay;

    client *m_sock;
};

#endif // CONTROLLER_H
