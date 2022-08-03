#include <QtDebug>
#include "controller.h"
#include "gpiocontrol.h"
#include "codingchannel.h"
#include "websock_client.h"

Controller::Controller(QObject *parent) : QObject(parent)
{
//   m_GpioControl = new GpioControl;
//   m_ch[0] = new CodingChannel;

    signalMapper = new QSignalMapper;

    gpio_delay.setSingleShot(true);
    gpio_delay.setInterval(100);

#if 1
    /* Create websocket client*/
     m_sock = new client;

    /* Create AFE Channel */
    //for(quint8 thread_count=0; thread_count<max_thread_count; thread_count++)
//    {
//        m_ch[thread_count] = new CodingChannel(thread_count);
//    }

     m_ch[0] = new CodingChannel(0);

    /* connect between websock thread and HID USB-SPI device thread */
    connect(signalMapper, SIGNAL(mapped(QString)), m_sock, SLOT(receiveFromAfe(QString)));
    connect(m_ch[0], SIGNAL(sig_transmitt_adc(QString)), signalMapper, SLOT(map()));
    signalMapper->setMapping(m_ch[0], "AFE_ch-1");

    //QObject::connect(m_ch[0], SIGNAL(sig_transmitt_adc(quint16)), m_sock, SLOT(receiveFromAfe(quint16)));
    //signalMapper->setMapping(m_ch[0], "AFE_ch-1");
    //connect(m_ch[0], SIGNAL(sig_transmitt_adc(quint16)), signalMapper, SLOT(map()));
    //connect(signalMapper, SIGNAL(mapped(quint16)), m_sock, SLOT(receiveFromAfe(quint16)));

    /* check adc value transmitt to websocket thread */
     m_ch[0]->adc_read_start();

#else
    connect(&gpio_delay, &QTimer::timeout,[=]()
    {        
        /* Create websock thread */
         m_sock = new client;
         m_sock_pThread = new QThread;
         m_sock->moveToThread(m_sock_pThread);
         connect(m_sock_pThread, &QThread::finished, m_sock, &QObject::deleteLater);
         m_sock_pThread->start();

         /* Create HID USB-SPI device thread */
         for(quint8 thread_count=0; thread_count<max_thread_count; thread_count++)
         {
             m_ch[thread_count] = new CodingChannel(thread_count);
             m_coding_pThread[thread_count] = new QThread;
             m_ch[thread_count]->moveToThread(m_coding_pThread[thread_count]);
             connect(m_coding_pThread[thread_count], &QThread::finished, m_ch[thread_count], &QObject::deleteLater);
             m_coding_pThread[thread_count]->start();
         }

         thread_comm();
    });

#endif

    /* Check the Board ID*/

    /* Poweron USB Hub*/
     gpio_delay.start();

}

void Controller::thread_comm()
{

}

Controller::~Controller()
{

}
