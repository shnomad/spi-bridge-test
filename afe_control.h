#ifndef AFECONTROL_H
#define AFECONTROL_H

#include <QVector>
#include <QTimer>
#include <QSocketNotifier>
#include "devices/spi_bridge/pl23d3.h"
#include "devices/dac/dac8562.h"
#include "devices/adc/ads8866.h"
#include "devices/adc/ads1120.h"
#include "devices/adc/ads130e08.h"
#include "jsondatahandle.h"
#include "common.h"
#include "mqtt.h"

//#define _USE_ADC_ADS1120_
#define _USE_ADC_ADS8866_
//#define _USE_ADC_ADS130E08_

class GpioSysFs;
class pl23d3;
class dac8562;

#ifdef _USE_ADC_ADS8866_
class ads8866;
#endif

#ifdef _USE_ADC_ADS1120_
class ads1120;
#endif

#ifdef _USE_ADC_ADS130E08_
class ads130e08;
#endif

class jsonDataHandle;

class AFEControl : public QObject
{
    Q_OBJECT
public:

    enum DAC_CH{
       CH_A =0x0,
       CH_B,
       CH_ALL
    };

        explicit AFEControl(QString, sys_cmd_resp*, QObject *parent = nullptr);
//      explicit AFEControl(QObject *parent = nullptr);

        ~AFEControl();    

signals:

Q_SIGNALS:
    void sig_resp_from_afe(QString);
    void sig_read_adc_manual();
    void sig_stop_read_adc_manual();

#ifdef _USE_ADC_ADS1120_
    void sig_read_adc_delay_complete();
#endif

public slots:
    void adc_read_start();

public Q_SLOTS:
    qint32 dac_init();
    qint32 dac_out(DAC_CH, float);
    qint32 dac_out(DAC_CH, quint16);
    qint32 adc_init();
    qint32 adc_read();
    void adc_read_ready();
    void dac_stop();
    void adc_read_stop();
    void adc_data_calculate(quint32, quint32);
    void cmd_from_TcpSocket(sys_cmd_resp *);    

private:    
    int hid_fd;
    QSocketNotifier *m_notify_hid;
    pl23d3 *m_usb_spi;
    dac8562 *m_dac;        
#ifdef _USE_ADC_ADS1120_
    ads1120 *m_adc;
#endif

#ifdef _USE_ADC_ADS8866_
    ads8866 *m_adc;
#endif

#ifdef _USE_ADC_ADS130E08_
    ads130e08 *m_adc;
#endif

    quint8 spi_read_buf[64]= {0x0,};

    QTimer *m_timer_adc, *m_timer_notice, *m_timer_read_adc_delay;

    QVector<quint16> adc_rawdata;           //save 1,200
    QVector<quint16> adc_average;           //save 40

    quint16 adc_min=0, adc_max=0;
    volatile quint8 total_count_loop=0;

    volatile quint32 read_adc_count =0;
    volatile quint16 adc_data_final = 0;
    quint16 adc_data_resp[12]= {0x0,};

    quint8 conding_ch_number =0;
    quint8 drdy_pin=1;

    jsonDataHandle *resp_to_json;
    sys_cmd_resp *afe_coding_ch_ctl;
    mqtt *send_error;

#ifdef _USE_ADC_ADS130E08_
    QSocketNotifier *drdy_notify;
    GpioSysFs *ads130e08_rdry;
    int drdy_fd;
    bool adc_capture_start=false;
#endif
};

#endif // AFECONTROL_H
