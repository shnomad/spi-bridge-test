#ifndef MQTT_H
#define MQTT_H

#include <QObject>
#include <QMqttClient>
#include <QTimer>
#include "jsondatahandle.h"
#include "common.h"

class mqtt : public QObject
{
    Q_OBJECT
public:
//    explicit mqtt(Coding_Channel_Ctl::channel, QObject *parent = nullptr);
     explicit mqtt(sys_cmd_resp *, QObject *parent = nullptr);
    ~mqtt();

signals:
    void sig_read_from_socket(const QByteArray &data);
    void sig_doconnect();
    void sig_cmd_to_afe(sys_cmd_resp *);
    void sig_launch_afe();
    void sig_afe_error(QString);

public slots:
    void setClientPort(int p);
    void ConnectBroker();
    void DisConnectBroker();

private slots:
    void updateLogStateChange();

    void brokerDisconnected();
    void brokerConnected();

    bool Publish(QString, QString, quint8, bool);
    bool Subscribe(QString);

    bool pub_response_topic(QString);
    bool pub_status_topic();
    bool sub_host_cmd_topic();

private:
    QMqttClient *m_client;

    QString client_id;
    QString pub_topic_status = "cgms/codingjig/afe/connected/";
    QString pub_topic_resp = "cgms/codingjig/afe/resp/";
    QString sub_topic_cmd = "cgms/codingjig/afe/cmd/";

    QTimer *connect_broker_timer, *pub_topic_status_timer, *pub_topic_resp_timer, *sub_topic_cmd_timer;

    /* Coding channel control parameter */
//    Coding_Channel_Ctl mqtt_coding_ch_ctl{}, mqtt_coding_ch_ctl_tmp{};
    jsonDataHandle *json_to_cmd;
    sys_cmd_resp *cmd_from_host;
};

#endif // MQTT_H
