#ifndef WEBSOCK_CLIENT_H
#define WEBSOCK_CLIENT_H

#include <QObject>
#include <QTimer>
#include <QtWebSockets/QWebSocket>

class client : public QObject
{
    Q_OBJECT
public:
    explicit client(QObject *parent = nullptr);
    ~client();
signals:

public slots:

Q_SIGNALS:
    void sig_transmitt_adc(QString);
    void closed();

private Q_SLOTS:
    void doConnect();
    void disConnectedFromHost();
    void onConnected();
    void onTextMessageReceived(QString message);
//    void receiveFromAfe(quint16);
    void receiveFromAfe(QString);

private:
    QWebSocket m_webSocket;
    QTimer *m_timer_doconnect;
//  QUrl m_url;
    QString url="ws://10.42.0.69:9001";
};

#endif // WEBSOCK_CLIENT_H
