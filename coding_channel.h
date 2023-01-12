#ifndef CODING_CHANNEL_THREAD_H
#define CODING_CHANNEL_THREAD_H

#include <QObject>
#include <QVector>
#include "common.h"
#include "afe_control.h"
#include "mqtt.h"

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
    mqtt *m_client;
    QVector <int *>afe_channel;
    volatile quint8 channel_number;
    sys_cmd_resp *afe_channel_info;
};

#endif // CODING_CHANNEL_THREAD_H
