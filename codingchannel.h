#ifndef CODINGCHANNEL_H
#define CODINGCHANNEL_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QSocketNotifier>
#include "devices/spi_bridge/prolific/pl23d3.h"
#include "devices/dac/dac8562/dac8562.h"
#include "devices/adc/ads8866.h"
#include "common.h"

class pl23d3;
class dac8562;
class ads8866;

class CodingChannel : public QObject
{
    Q_OBJECT
public:

    enum DAC_CH{
       CH_A =0x0,
       CH_B,
       CH_ALL
    };

    explicit CodingChannel(quint8, QObject *parent = nullptr);
        ~CodingChannel();
        qint32 dac_init();
        void adc_read_start();
signals:

Q_SIGNALS:
    void sig_transmitt_adc(QString);
//  void sig_transmitt_adc(quint16);

public slots:
    qint32 dac_out(DAC_CH, float);
    qint32 adc_read();
    void adc_read_ready();
    void adc_data_transmitt();
    void dac_stop();
    void adc_read_stop();

private:
    int hid_fd;
    QSocketNotifier *m_notify_hid;
    pl23d3 *m_usb_spi;
    dac8562 *m_dac;
    ads8866 *m_adc;
    quint8 spi_read_buf[64]= {0x0,};

    QTimer m_timer_adc, m_timer_adc_transmitt;

    QVector<quint16> adc_rawdata;           //save 1,200
    QVector<quint16> adc_average;           //save 40

    volatile quint32 read_adc_count =0;
    volatile quint16 adc_data_final = 0;
};

#endif // CODINGCHANNEL_H
