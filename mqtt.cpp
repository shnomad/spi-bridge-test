#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QDebug>
#include "mqtt.h"

//mqtt::mqtt(QObject *parent) : QObject(parent)
mqtt::mqtt(Coding_Channel_Ctl::channel n_ch,QObject *parent) : QObject(parent)
{
    client_id = "CH_" + QString("%1").arg(n_ch, 3, 10, QChar('0'));

    QString hostname ="10.42.0.69";
 // QString hostname ="192.168.137.1";
    quint16 portnumber = 1883;

    m_client = new QMqttClient(this);
    m_client->setHostname(hostname);
    m_client->setPort(portnumber);
    m_client->setClientId(client_id);
    m_client->setKeepAlive(120);

    const QString willtopic = "cgms/codingjig/afe/connected/"+client_id;
    QString willmsg ="false";

    m_client->setWillTopic(willtopic);
    m_client->setWillMessage(willmsg.toStdString().c_str());

    connect_broker_timer = new QTimer;
    connect_broker_timer->setSingleShot(true);
    connect_broker_timer->setInterval(500);

    pub_topic_status_timer = new QTimer;
    pub_topic_status_timer->setSingleShot(true);
    pub_topic_status_timer->setInterval(100);

    pub_topic_resp_timer = new QTimer;
    pub_topic_resp_timer->setSingleShot(true);
    pub_topic_resp_timer->setInterval(300);

    sub_topic_cmd_timer = new QTimer;
    sub_topic_cmd_timer->setSingleShot(true);
    sub_topic_cmd_timer->setInterval(300);

    connect(connect_broker_timer, SIGNAL(timeout()), this, SLOT(ConnectBroker()));
    connect(pub_topic_status_timer, SIGNAL(timeout()), this, SLOT(pub_status_topic()));
//  connect(pub_topic_resp_timer, SIGNAL(timeout()), this, SLOT(pub_response_topic()));
    connect(sub_topic_cmd_timer, SIGNAL(timeout()), this, SLOT(sub_host_cmd_topic()));

    connect(m_client, &QMqttClient::stateChanged, this, &mqtt::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &mqtt::brokerDisconnected);
    connect(m_client, &QMqttClient::connected, this, &mqtt::brokerConnected);

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {

        const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(" Received Topic: ")
                    + topic.name()
                    + QLatin1String(" Message: ")
                    + message
                    + QLatin1Char('\n');
        qDebug()<<client_id<<":"<<content;

        mqtt_coding_ch_ctl = json_to_cmd->parse(message);

        if(mqtt_coding_ch_ctl.m_cmd == Coding_Channel_Ctl::CMD_CHECK_AFE_CONNECTED)
        {
            pub_status_topic();
        }
        else
        {
            emit sig_cmd_to_afe(mqtt_coding_ch_ctl);
        }

//      emit sig_cmd_to_afe(json_to_cmd->parse(message));

    });

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
         const QString content = QDateTime::currentDateTime().toString()
                     + QLatin1String(" PingResponse")
                     + QLatin1Char('\n');
        qDebug()<<client_id<<":"<<content;
     });

    updateLogStateChange();

    connect_broker_timer->start();

//  ConnectBroker();
}

void mqtt::setClientPort(int p)
{
    m_client->setPort(p);
}

void mqtt::ConnectBroker()
{

    if (m_client->state() == QMqttClient::Disconnected)
    {
        m_client->connectToHost();
    }
    else
    {
        m_client->disconnectFromHost();
    }
}

void mqtt::DisConnectBroker()
{
    exit(0);
}

void mqtt::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    qDebug()<<content;
}

void mqtt::brokerDisconnected()
{
    qDebug()<<client_id<<": DisConnected";
}

void mqtt::brokerConnected()
{
    qDebug()<<"Connected";
    qDebug()<<"client_id :"<<client_id;

    pub_topic_status_timer->start();
    sub_topic_cmd_timer->start();

// on_buttonPublish_clicked(pub_topic_status + client_id, "TRUE", 1, false);
}

bool mqtt::Publish(QString topic, QString msg, quint8 Qos, bool retain)
{
    if (m_client->publish(QMqttTopicName(topic), msg.toUtf8(), Qos, retain) == -1)
    {
        qDebug()<<"Could not publish message";

        return false;
    }

    return true;
}

bool mqtt::Subscribe(QString topic)
{
    auto subscription = m_client->subscribe(topic);

    if (!subscription)
    {
           qDebug()<<"Could not subscribe. Is there a valid connection?";

           return false;
     }

    return true;
}

bool mqtt::pub_status_topic()
{
    if(Publish(pub_topic_status + client_id, "true", 1, false))
    {
        qDebug()<<client_id<<" :pub_status_topic success";

        sub_topic_cmd_timer->start();

        return true;
    }

    qDebug()<<client_id <<" :pub_status_topic failed";

    return false;
}

bool mqtt::pub_response_topic(QString msg)
{
    if(Publish(pub_topic_resp + client_id, msg, 1, false))
    {
        qDebug()<<client_id<<" :pub_response_topic success";

        return true;
    }

    qDebug()<<client_id<<" :pub_response_topic failed";

    return false;
}

bool mqtt::sub_host_cmd_topic()
{
    if(Subscribe(sub_topic_cmd + client_id))
    {
        qDebug()<<client_id<<" :sub_host_cmd_topic success";

        pub_topic_resp_timer->start();

        return true;
    }

    qDebug()<<client_id<<" :sub_host_cmd_topic failed";

    return false;
}

mqtt::~mqtt()
{

}
