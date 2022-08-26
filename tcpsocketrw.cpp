#include "tcpsocketrw.h"
#include "common.h"

//TcpSocketRW::TcpSocketRW(local_network_info *net_info, QObject *parent) : QObject(parent)
TcpSocketRW::TcpSocketRW(local_network_info *net_info, Coding_Channel_Ctl::channel n_ch, QObject *parent) : QObject(parent)
{
    qInfo() << "TCPSocketRW Started"<<Qt::endl;

    m_local_network_info = new local_network_info;
    m_local_network_info = net_info;

    tcp_coding_ch_ctl.m_ch = n_ch;

    json_to_cmd = new jsonDataHandle;

    /*connecting to Host Timer settings */
    m_timer_doconnect = new QTimer;
    m_timer_doconnect->setSingleShot(true);
    m_timer_doconnect->setInterval(100);

    m_timer_close = new QTimer;
    m_timer_close->setSingleShot(true);
    m_timer_close->setInterval(1000*30);

    connect(this, SIGNAL(sig_connetion_status(socket_status)), this, SLOT(manage_connection(socket_status)));    
    connect(m_timer_doconnect, SIGNAL(timeout()), this, SLOT(doConnect()));
    connect(m_timer_close, SIGNAL(timeout()), this, SLOT(sock_disconnect()));

    m_timer_doconnect->start();
}

void TcpSocketRW::doConnect()
{
    qInfo() <<"TCP Socket doConnet"<<Qt::endl;

    qsocket = new QTcpSocket(this);

    if(qsocket->bind(m_local_network_info->TCP_Local_port, QAbstractSocket::ShareAddress))
    {
          qDebug() << "Binded Correctly" << Qt::endl;
          qDebug() << "Local IP Address : " << QHostAddress(m_local_network_info->TCP_Local_address).toString() <<", Port :" <<m_local_network_info->TCP_Local_port << Qt::endl;

          connect(qsocket, SIGNAL(connected()),this, SLOT(connected()));
          connect(qsocket, SIGNAL(disconnected()),this, SLOT(disconnectedfromHost()));
          connect(qsocket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
          connect(qsocket, SIGNAL(readyRead()),this, SLOT(readyRead()));
    }

    qsocket->connectToHost(QHostAddress(m_local_network_info->TCP_Remote_address), m_local_network_info->TCP_Remote_port);

    if(qsocket->waitForConnected(500))
    {
            qDebug()<<"Socket connect Success"<<Qt::endl;

             if(tcp_coding_ch_ctl.m_ch == 1 || tcp_coding_ch_ctl.m_ch == 4 || tcp_coding_ch_ctl.m_ch == 7)
             {
                 qDebug()<<"Socket Close count down start : tcp_coding_ch_ctl.m_ch" <<Qt::endl;
//               m_timer_close->start();
             }
    }
    else
    {
        emit sig_connetion_status(ERROR_CONNECT_HOST);
    }

#if 0

   if(do_console_command_get_result("ss -at src = "+QHostAddress(m_ch_info_tcp->local_net.TCP_Local_address).toString()+":"+QString("%1").arg(m_ch_info_tcp->local_net.TCP_Local_port,5,'g',-1,'0'), \
          QHostAddress(m_ch_info_tcp->local_net.TCP_Local_address).toString()+":"+QString("%1").arg(m_ch_info_tcp->local_net.TCP_Local_port,5,'g',-1,'0')))
   {
       Log()<<"Previous Socket or port exsist";

       if(qsocket->state() != QAbstractSocket::ConnectedState)
            emit sig_connetion_status(ERROR_SOCKET_EXSIST);
   }
   else
   {
        qsocket = new QTcpSocket(this);

        if(qsocket->bind(QHostAddress(m_ch_info_tcp->local_net.TCP_Local_address), m_ch_info_tcp->local_net.TCP_Local_port, QAbstractSocket::ShareAddress))
        {
              qDebug() << "Binded Correctly"<< Qt::endl;
              qDebug() <<  QHostAddress(m_ch_info_tcp->local_net.TCP_Local_address).toString() << ":" << m_ch_info_tcp->local_net.TCP_Local_port << Qt::endl;
        }
        else
        {
              qDebug() << "Unable to bind"<<QHostAddress(m_ch_info_tcp->local_net.TCP_Local_address) << ":" << m_ch_info_tcp->local_net.TCP_Local_port << Qt::endl;
        }

        connect(qsocket, SIGNAL(connected()),this, SLOT(connected()));
        connect(qsocket, SIGNAL(disconnected()),this, SLOT(disconnectedfromHost()));
        connect(qsocket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
        connect(qsocket, SIGNAL(readyRead()),this, SLOT(readyRead()));

        qsocket->connectToHost(QHostAddress(m_ch_info_tcp->local_net.TCP_Remote_address), m_ch_info_tcp->local_net.TCP_Remote_port);

        if(qsocket->waitForConnected(500))
        {
            /*Check Host Connection*/
            if(do_console_command_get_result("ss -t4 state established '( dst = "+QHostAddress(m_ch_info_tcp->local_net.TCP_Remote_address).toString()+":"+QString("%1").arg(m_ch_info_tcp->local_net.TCP_Remote_port,4,'g',-1,'0')+" )'", \
                  QHostAddress(m_ch_info_tcp->local_net.TCP_Local_address).toString()+":"+QString("%1").arg(m_ch_info_tcp->local_net.TCP_Local_port,5,'g',-1,'0')))
            {
                qDebug()<<"Socket connect Success"<<Qt::endl;
            }
            else
            {
                emit sig_connetion_status(ERROR_CONNECT_HOST);
            }

        }
        else
        {
            emit sig_connetion_status(ERROR_CONNECT_HOST);
        }
  }
#endif
}

void TcpSocketRW::connected()
{
     QString socket_connected;

     qDebug() << "IP Address : " << QHostAddress().toString() <<", Port :" <<m_local_network_info->TCP_Local_port << Qt::endl;

     QString client_sock = "IP Address : " + QHostAddress(m_local_network_info->TCP_Local_address).toString() + ":" + QString::number(m_local_network_info->TCP_Local_port, 10);

     tcp_coding_ch_ctl.m_resp = Coding_Channel_Ctl::RESP_SOCKET_CONNECTED;

//   socket_connected = tojson->create(tcp_coding_ch_ctl.m_ch, tcp_coding_ch_ctl.m_resp, nullptr, nullptr);

//   if(m_local_network_info->TCP_Local_port == 60001)
//       writedata(socket_connected.toUtf8());
}

void TcpSocketRW::sock_disconnect()
{
    qsocket->close();
}

void TcpSocketRW::disconnectedfromHost()
{
    qInfo() << "disconnected from Host";
    removeSocket(true);
    emit sig_connetion_status(ERROR_DISCONNECT_FROM_HOST);
}

void TcpSocketRW::removeSocket(bool qtsock)
{    
    if(qtsock)
    {
        qsocket->flush();
        qsocket->close();
        qsocket->abort();
        qsocket->deleteLater();
    }
}

void TcpSocketRW::writedata(const QByteArray &data)
{
    if(qsocket->state() == QAbstractSocket::ConnectedState)
        qsocket->write(data);
}

void TcpSocketRW::bytesWritten(qint64 bytes)
{
 //   qDebug() << bytes << " bytes written...";
}

void TcpSocketRW::readyRead()
{
    /* Receive message or command from TCP Server*/
    m_readData = qsocket->readAll();

//  Log()<< "Port" << m_local_network_info->TCP_Local_port << "Server sent:"<< QByteArray(m_readData)<<"\n";
//  json_to_cmd->parse(QByteArray(m_readData));

    /* send command to afe control */
    emit sig_cmd_to_afe(json_to_cmd->parse(QByteArray(m_readData)));
}


void TcpSocketRW::manage_connection(socket_status status)
{
    qWarning() << "manage socket connection";

    switch (status)
    {
        case ERROR_SOCKET_CREATE:
        case ERROR_BIND:
        case ERROR_SOCKET_EXSIST:

        break;

        case ERROR_CONNECT_HOST:
        case ERROR_DISCONNECT_FROM_HOST:
        case ERROR_DISCONNECT_FROM_METER:

            disconnect(qsocket, SIGNAL(connected()),this, SLOT(connected()));
            disconnect(qsocket, SIGNAL(disconnected()),this, SLOT(disconnectedfromHost()));
            disconnect(qsocket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
            disconnect(qsocket, SIGNAL(readyRead()),this, SLOT(readyRead()));

        break;
    }

    m_timer_doconnect->setInterval(3000);
    m_timer_doconnect->start();

    emit sig_doconnect();
}

void TcpSocketRW::resp_from_afe(QString response)
{
    /* Send Response or data to TCP Server*/
    Log()<<response;

    writedata(response.toStdString().c_str());
}

TcpSocketRW::~TcpSocketRW()
{
    delete m_timer_doconnect;
}
