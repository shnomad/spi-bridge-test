#ifndef AFECONTROL_H
#define AFECONTROL_H

#include <QVector>
#include <QTimer>
#include <QSocketNotifier>
#include "devices/spi_bridge/pl23d3.h"
#include "devices/dac/dac8562.h"
#include "devices/adc/ads8866.h"
#include "jsondatahandle.h"
#include "common.h"

class pl23d3;
class dac8562;
class ads8866;
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

    explicit AFEControl(QString, Coding_Channel_Ctl::channel, QObject *parent = nullptr);
        ~AFEControl();    

signals:

Q_SIGNALS:
    void sig_cmd_to_afe(Coding_Channel_Ctl);
    void sig_resp_from_afe(QString);
    void sig_read_adc_manual();
    void sig_stop_read_adc_manual();

public slots:
    void adc_read_start();

public Q_SLOTS:
    qint32 dac_init();
    qint32 dac_out(DAC_CH, float);
    qint32 adc_read();
    void adc_read_ready();
//  void adc_data_transmitt();
    void dac_stop();
    void adc_read_stop();
    void cmd_from_TcpSocket(Coding_Channel_Ctl);

private:
    int hid_fd;
    QSocketNotifier *m_notify_hid;
    pl23d3 *m_usb_spi;
    dac8562 *m_dac;
    ads8866 *m_adc;
    quint8 spi_read_buf[64]= {0x0,};

//    QTimer *m_timer_adc, *m_timer_adc_transmitt, *m_timer_notice;
    QTimer *m_timer_adc, *m_timer_notice;

    QVector<quint16> adc_rawdata;           //save 1,200
    QVector<quint16> adc_average;           //save 40

    quint16 adc_min=0, adc_max=0;
    volatile quint8 total_count_loop=0;

    volatile quint32 read_adc_count =0;
    volatile quint16 adc_data_final = 0;
    quint16 adc_data_resp[12]= {0x0,};
    quint16 dac_value[2]={0x0,};

    Coding_Channel_Ctl afe_coding_ch_ctl{};
    quint8 conding_ch_number =0;

    jsonDataHandle *resp_to_json;
};

#endif // AFECONTROL_H
