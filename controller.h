#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
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
    enum DAC_CH{
       CH_A =0x0,
       CH_B,
       CH_ALL
    };

    explicit Controller(QObject *parent = nullptr);
    qint32 dac_init();
    void adc_read_start();
    ~Controller();

signals:

public slots:

    qint32 dac_out(DAC_CH, float);
    qint32 adc_read();
    void adc_read_ready();
    void adc_data_transmitt();
    void dac_stop();
    void adc_read_stop();

private:

    pl23d3 *m_usb_spi;
    dac8562 *m_dac;
    ads8866 *m_adc;

    int fd_hid;

    quint8 read_adc[2] ={0x0,};
    float voltage;
    QByteArray  m_tranferHost;

    quint8 read_buf[64] ={0x0,};
};

#endif // CONTROLLER_H
