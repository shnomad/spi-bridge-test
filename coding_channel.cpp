
#include "coding_channel.h"

coding_channel::coding_channel(quint8 thread_seq, QObject *parent) : QObject(parent)
{
    afe_channel_info = new sys_cmd_resp;   


    QList<QString> hid_port_name = {"usb-spi-1","usb-spi-2","usb-spi-3","usb-spi-4","usb-spi-5", \
                            "usb-spi-6","usb-spi-7","usb-spi-8","usb-spi-9","usb-spi-10", \
                            "usb-spi-11","usb-spi-12","usb-spi-13","usb-spi-14","usb-spi-15"};

    set_afe_number();

    /* Read Board ID/Number,read from GPIO, Later need to implement */
    quint8 Board_Number = 0x0;

    channel_number = get_afe_number(Board_Number, thread_seq);

    afe_channel_info->m_ch = static_cast<sys_cmd_resp::channel>(channel_number);
    afe_channel_info->board_number = Board_Number;

    m_client = new mqtt(afe_channel_info);

   /* Dummy Channel Test */
    m_afe_control = new AFEControl(hid_port_name.at(thread_seq), afe_channel_info);

    connect(m_client, SIGNAL(sig_cmd_to_afe(sys_cmd_resp*)), m_afe_control, SLOT(cmd_from_TcpSocket(sys_cmd_resp*)));
    connect(m_afe_control, SIGNAL(sig_resp_from_afe(QString)), m_client, SLOT(pub_response_topic(QString)));
}

void coding_channel::set_afe_number()
{

    int total_channel[12][15] ={\
                                {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}, \
                                {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30}, \
                                {31,32,33,34,35,36,37,38,39,40,41,42,43,44,45}, \
                                {46,47,48,49,50,51,52,53,54,55,56,57,58,59,60}, \
                                {61,62,63,64,65,66,67,68,69,70,71,72,73,74,75}, \
                                {76,77,78,79,80,81,82,83,84,85,86,87,88,89,90}, \
                                {91,92,93,94,95,96,97,98,99,100,101,102,103,104,105}, \
                                {106,107,108,109,110,111,112,113,114,115,116,117,118,119,120}, \
                                {121,122,123,124,125,126,127,128,129,130,131,132,133,134,135}, \
                                {136,137,138,139,140,141,142,143,144,145,146,147,148,149,150}, \
                                {151,152,153,154,155,156,157,158,159,160,161,162,163,164,165}, \
                                {166,167,168,169,170,171,172,173,174,175,176,177,178,179,180}, \
                                };

    for(quint8 board_num=0; board_num<12; board_num++)
    {
        afe_channel.push_back(total_channel[board_num]);
    }

}

quint8 coding_channel::get_afe_number(quint8 board_number, quint8 thread_number)
{
    return static_cast<quint8>(afe_channel.value(board_number)[thread_number]);
}

coding_channel::~coding_channel()
{

}
