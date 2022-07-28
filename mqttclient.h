#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include "QtMqtt/qmqttclient.h"

class mqttclient : public QObject
{
    Q_OBJECT
public:
    explicit mqttclient(QObject *parent = nullptr);
    void brokerConnect();
    void brokerDisconnected();
    void getSubscribe(QString);
    void setPublish(QString, QString);
    void pingRequest();
    ~mqttclient();

signals:

public slots:
    void updateLogStateChange();

private:

    QMqttClient *m_client;

};

#endif // MQTTCLIENT_H
