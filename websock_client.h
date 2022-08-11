#ifndef WEBSOCK_CLIENT_H
#define WEBSOCK_CLIENT_H

#include <QTimer>
#include "common.h"
#include "jsondatahandle.h"
#include <QtWebSockets/QWebSocket>

class jsonDataHandle;

class WebSockClient : public QObject
{
    Q_OBJECT
public:
    explicit WebSockClient(Coding_Channel_Ctl::channel,QObject *parent = nullptr);
    ~WebSockClient();
signals:

public slots:

Q_SIGNALS:
    void sig_cmd_resp(Coding_Channel_Ctl);
    void closed();

private Q_SLOTS:
    void doConnect();
    void disConnectedFromHost();
    void onConnected();
    void onTextMessageReceived(QString message);
    void cmd_resp_with_coding_channel(Coding_Channel_Ctl);

private:
    QWebSocket m_webSocket;
    QTimer *m_timer_doconnect;
    QString url="ws://10.42.0.69:9001";

    Coding_Channel_Ctl m_ch_ctl_param{};
    jsonDataHandle *m_json_data;
};

#endif // WEBSOCK_CLIENT_H
