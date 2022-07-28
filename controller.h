#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
//#include "devices/spi_bridge/ftdi/ft4222.h"
#include "devices/dac/dac8562/dac8562.h"
#include "devices/adc/ads8866.h"

//class ft4222;
class pl23d3;
class dac8562;
class ads8866;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    void ReadyRead();
    ~Controller();

signals:

public slots:

private:

    pl23d3 *m_usb_spi;
    dac8562 *m_dac;
    ads8866 *m_adc;

    quint8 read_adc[2] ={0x0,};
    float voltage;
    QByteArray  m_tranferHost;

    quint8 read_buf[64] ={0x0,};
    int fd_hid;        

    //mqttclient *m_mqtt_Client;

};

#endif // CONTROLLER_H
