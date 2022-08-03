#include "websock_client.h"
#include "common.h"

client::client(QObject *parent) : QObject(parent)
{    
    m_timer_doconnect = new QTimer;
    m_timer_doconnect->setSingleShot(true);
    m_timer_doconnect->setInterval(100);

    connect(m_timer_doconnect, SIGNAL(timeout()), this, SLOT(doConnect()));
    connect(&m_webSocket, &QWebSocket::connected, this, &client::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &client::closed);

//  m_timer_doconnect->start();
}

void client::onConnected()
{
    Log() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &client::onTextMessageReceived);
    m_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
}

void client::doConnect()
{
    m_webSocket.open(QUrl(url));
}

void client::disConnectedFromHost()
{

}

void client::onTextMessageReceived(QString message)
{
    Log() << "Message received:" << message;
    m_webSocket.close();
}

//void client::receiveFromAfe(quint16 data)
void client::receiveFromAfe(QString data)
{
    Log()<<"Received from ADC :" <<data;
}

client::~client()
{

}
