#include <QDateTime>
#include <QDebug>
#include "mqttclient.h"


mqttclient::mqttclient(QObject *parent) : QObject(parent)
{
    m_client = new QMqttClient(this);

    m_client->setHostname("10.42.0.69");
    m_client->setPort(1883);

    connect(m_client, &QMqttClient::stateChanged, this, &mqttclient::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &mqttclient::brokerDisconnected);

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic)
    {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" Received Topic: ")
                    + topic.name()
                    + QLatin1String(" Message: ")
                    + message
                    + QLatin1Char('\n');
    });

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]()
    {
        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" PingResponse")
                    + QLatin1Char('\n');
    });

    updateLogStateChange();
}

void mqttclient::brokerConnect()
{
    m_client->connectToHost();
}

void mqttclient::brokerDisconnected()
{
    qDebug()<<"broker disconnected";
}

void mqttclient::getSubscribe(QString topic)
{
    m_client->subscribe(topic);
}

void mqttclient::setPublish(QString topic, QString msg)
{
    m_client->publish(topic);
}

void mqttclient::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                       + QLatin1String(": State Change")
                       + QString::number(m_client->state())
                       + QLatin1Char('\n');
}

void mqttclient::pingRequest()
{
    m_client->requestPing();
}

mqttclient::~mqttclient()
{

}
