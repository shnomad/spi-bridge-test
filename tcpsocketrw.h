#ifndef WEBSOCK_CLIENT_H
#define WEBSOCK_CLIENT_H

#include <QTcpSocket>
#include <QAbstractSocket>
#include <QtNetwork>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "jsondatahandle.h"
#include "common.h"

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class jsonDataHandle;

class TcpSocketRW : public QObject
{
    Q_OBJECT
public:
//      explicit TcpSocketRW(quint16, QObject *parent = nullptr);
//      explicit TcpSocketRW(local_network_info *, QObject *parent = nullptr);
        explicit TcpSocketRW(local_network_info *, Coding_Channel_Ctl::channel ,QObject *parent = nullptr);
            ~TcpSocketRW();

    enum socket_status
    {
        ERROR_SOCKET_CREATE =1,
        ERROR_BIND,
        ERROR_CONNECT_HOST,
        ERROR_DISCONNECT_FROM_HOST,
        ERROR_DISCONNECT_FROM_METER,
        ERROR_SOCKET_EXSIST
    };

signals:
    void sig_read_from_socket(const QByteArray &data);
    void sig_doconnect();
    void sig_connetion_status(socket_status);
    void sig_cmd_to_afe(Coding_Channel_Ctl);
//  void sig_send_ch_info(meter_ch_info);

public slots:

    void doConnect();
    void connected();
    void disconnectedfromHost();
    void sock_disconnect();
    void removeSocket(bool);
    void writedata(const QByteArray &data);
    void bytesWritten(qint64 bytes);
    void readyRead();
    void manage_connection(socket_status);
    void resp_from_afe(QString);

private:

    QByteArray m_readData;
    QTimer *m_timer_doconnect, *m_timer_close;

    /*TCP Socket Create*/
    QTcpSocket *qsocket;    
    local_network_info *m_local_network_info;

    /* Coding channel parameter */
    Coding_Channel_Ctl tcp_coding_ch_ctl{};

    jsonDataHandle *json_to_cmd;
};
#endif // WEBSOCK_CLIENT_H
