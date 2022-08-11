#ifndef CODING_CHANNEL_THREAD_H
#define CODING_CHANNEL_THREAD_H

#include <QObject>
#include <QVector>
#include "common.h"

class AFEControl;
class WebSockClient;

class coding_channel : public QObject
{
    Q_OBJECT
public:
    explicit coding_channel(quint8, QObject *parent = nullptr);
    ~coding_channel();
    void set_afe_number();
    quint8 get_afe_number(quint8, quint8);

signals:

public slots:

private:

    AFEControl *m_afe_control;
    WebSockClient *m_websock_client;
    Coding_Channel_Ctl m_coding_ch_ctl_param{};

    QVector <int *>afe_channel;
};

#endif // CODING_CHANNEL_THREAD_H
