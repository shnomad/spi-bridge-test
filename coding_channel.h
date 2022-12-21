#ifndef CODING_CHANNEL_THREAD_H
#define CODING_CHANNEL_THREAD_H

#include <QObject>
#include <QVector>
#include "common.h"
#include "mqtt.h"

#define LocalPort_pre  60000

class AFEControl;
//class TcpSocketRW;

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
//  TcpSocketRW *m_tcpsocket;
    mqtt *m_ClientMqtt;

    Coding_Channel_Ctl m_coding_ch_ctl{};
    QVector <int *>afe_channel;
    volatile quint8 channel_number;

//  local_network_info *net_info;
};

#endif // CODING_CHANNEL_THREAD_H
