#include <QtDebug>
#include "controller.h"
//#include "coding_channel.h"
#include "devices/adc/ads1120.h"
//#include "devices/temperature/ds18b20.h"
//#include "gpiocontrol.h"

Controller::Controller(QObject *parent) : QObject(parent)
{

    for(quint8 thread_count=0; thread_count<max_thread_count; thread_count++)
      {
          m_ch[thread_count] = new coding_channel(thread_count);
          m_pthread[thread_count] = new QThread;
          m_ch[thread_count]->moveToThread(m_pthread[thread_count]);
          connect(m_pthread[thread_count], &QThread::finished, m_ch[thread_count], &QObject::deleteLater);
          m_pthread[thread_count]->start();
      }

}

Controller::~Controller()
{


}
