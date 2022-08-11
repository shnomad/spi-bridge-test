#include "websock_client.h"
#include "common.h"

WebSockClient::WebSockClient(Coding_Channel_Ctl::channel ch, QObject *parent) : QObject(parent)
{    
    /* channel number of this websock object*/
    m_ch_ctl_param.m_ch = ch;
    m_json_data = new jsonDataHandle;

    m_timer_doconnect = new QTimer;
    m_timer_doconnect->setSingleShot(true);
    m_timer_doconnect->setInterval(500);

    connect(m_timer_doconnect, SIGNAL(timeout()), this, SLOT(doConnect()));
    connect(&m_webSocket, &QWebSocket::connected, this, &WebSockClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WebSockClient::closed);

//  m_timer_doconnect->start();
}

void WebSockClient::onConnected()
{
    Log() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebSockClient::onTextMessageReceived);
    m_webSocket.sendTextMessage(QStringLiteral("Hello, world! from channel"));
}

void WebSockClient::doConnect()
{
    m_webSocket.open(QUrl(url));
}

void WebSockClient::disConnectedFromHost()
{
    Log() << "disConnectedFromHost";
}

void WebSockClient::onTextMessageReceived(QString message)
{    
    Log() <<m_ch_ctl_param.m_ch <<":Message received:" << message;
   // m_webSocket.close();
}

void WebSockClient::cmd_resp_with_coding_channel(Coding_Channel_Ctl m_ch_ctl)
{    
    switch(m_ch_ctl.m_resp)
    {
        case Coding_Channel_Ctl::resp::AFE_START:

            m_ch_ctl_param.m_ch = m_ch_ctl.m_ch;
            m_ch_ctl_param.m_cmd = Coding_Channel_Ctl::START_DAC;
            emit sig_cmd_resp(m_ch_ctl_param);

        break;

        case Coding_Channel_Ctl::resp::AFE_STOP:

        break;

        case Coding_Channel_Ctl::resp::START_DAC_SUCCESS:

            m_ch_ctl_param.m_ch = m_ch_ctl.m_ch;
            m_ch_ctl_param.m_cmd = Coding_Channel_Ctl::START_ADC;

            m_json_data->create(m_ch_ctl.m_ch, 0x0, m_ch_ctl.dac_value_a, m_ch_ctl.dac_value_b);

            emit sig_cmd_resp(m_ch_ctl_param);

        break;

        case Coding_Channel_Ctl::resp::STOP_DAC_SUCCESS:

        break;

        case Coding_Channel_Ctl::resp::START_ADC_SUCCESS:

        break;

        case Coding_Channel_Ctl::resp::READ_ADC:

            Log()<<"\n"<<"Channel Number : "<< m_ch_ctl.m_ch <<"\n"<< "Received ADC value :"<< QString("%1").arg(m_ch_ctl.adc_read_value, 0, 16);
            m_json_data->create(m_ch_ctl.m_ch, m_ch_ctl.adc_read_value, m_ch_ctl.dac_value_a, m_ch_ctl.dac_value_b);

        break;

        case Coding_Channel_Ctl::resp::STOP_ADC_SUCCESS:

        break;

        case Coding_Channel_Ctl::resp::START_DAC_FAIL:

        break;

        case Coding_Channel_Ctl::resp::START_ADC_FAIL:

        break;

        case Coding_Channel_Ctl::resp::UNKNOW_RESPONSE:

        break;

        default:

        break;
    }
}

WebSockClient::~WebSockClient()
{

}
