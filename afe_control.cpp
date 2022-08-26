#include "afe_control.h"
#include <bits/stdc++.h>

AFEControl::AFEControl(QString hid_port_name, Coding_Channel_Ctl::channel ch, QObject *parent) : QObject(parent)
{    
    m_usb_spi = new pl23d3;
    m_dac = new dac8562;
    m_adc = new ads8866;

    m_timer_adc = new QTimer;
    m_timer_adc_transmitt = new QTimer;
    m_timer_notice = new QTimer;

    QString hidpath = "/dev/"+hid_port_name;

    hid_fd = m_usb_spi->SPI_Open(hidpath.toStdString().c_str());

    if(hid_fd < 0)
    {
        Log()<<"HID Device Open Fail";
        exit(0);
    }

    afe_coding_ch_ctl.m_ch = ch;

    m_usb_spi->SPI_Master_Init(hid_fd);

    dac_init();

    /* Socket notifier for SPI read interrupt*/
    m_notify_hid = new QSocketNotifier(hid_fd, QSocketNotifier::Read, this);

    connect(m_notify_hid, SIGNAL(activated(int)), this, SLOT(adc_read_ready()));

    m_notify_hid->setEnabled(true);

    /* read ADC started by timer*/
    m_timer_adc->setSingleShot(true);
    m_timer_adc->setInterval(0);

    QObject::connect(m_timer_adc, SIGNAL(timeout()), this, SLOT(adc_read()));

    /* ADC data transmittion timer, after ADC capure & calculate complete it'll start */
    m_timer_adc_transmitt->setSingleShot(true);
    m_timer_adc_transmitt->setInterval(0);

    QObject::connect(m_timer_adc_transmitt, SIGNAL(timeout()), this, SLOT(adc_data_transmitt()));

    /* notice to websock thread "I'm Ready"*/
    m_timer_notice->setSingleShot(true);
    m_timer_notice->setInterval(100);

    connect(m_timer_notice, &QTimer::timeout,[=]()
          {
                afe_coding_ch_ctl.m_ch = static_cast<Coding_Channel_Ctl::channel>(conding_ch_number);
                afe_coding_ch_ctl.m_resp = Coding_Channel_Ctl::RESP_AFE_READY_SUCCESS;
                /**/
//              emit sig_resp_from_afe(afe_coding_ch_ctl);
          });

     resp_to_json = new jsonDataHandle;

//  m_timer_notice->start();
}

qint32 AFEControl::dac_init()
{
    /* DAC Initialize */

    qint32 result;

    result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG));

    result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B));

    result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->DAC_WR_REG(CMD_GAIN, DATA_GAIN_B1_A1));

    result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS));

    return result;
}

qint32 AFEControl::dac_out(AFEControl::DAC_CH ch, float value)
{
    /* DAC voltage out */

    qint32 result;

   if(ch == AFEControl::DAC_CH::CH_A)
   {
         result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->writeA(value));
         afe_coding_ch_ctl.dac_value_a = value;
   }
   else if(ch == AFEControl::DAC_CH::CH_B)
   {
         result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->writeB(value));
         afe_coding_ch_ctl.dac_value_b = value;
   }
   else
   {
        result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->writeALL(value));
   }

   return result;
}

void AFEControl::adc_read_start()
{
    read_adc_count =0;
    m_notify_hid->setEnabled(true);
    m_timer_adc->start();    
}

qint32 AFEControl::adc_read()
{
    qint32 result;
    result = m_usb_spi->SPI_Single_Read(hid_fd);

    return result;
}

void AFEControl::adc_read_ready()
{
    qint32  result;
    quint16 adc_value = 0;
    quint64 adc_average_1st=0, adc_average_2nd=0;

   if(read_adc_count < 1200)
   {
         /* Read ADC data and store */
         result = read(hid_fd, spi_read_buf, 64);

        if(spi_read_buf[0] == 0xb3 && spi_read_buf[63] == 0x2)
        {
            adc_value |= spi_read_buf[1] << 8;
            adc_value |= spi_read_buf[2];

            adc_rawdata.append(adc_value);

            read_adc_count++;
        }

//      if(read_adc_count ==1 || read_adc_count == 1200)
//          Log()<<"["<< read_adc_count <<"]"<< " Read ADC Value :"<<  QString("%1").arg(adc_value, 0, 16);

        adc_value = 0;

        m_timer_adc->start();
   }
   else
   {
        m_notify_hid->setEnabled(false);

       /* 1. Calculate the average value of every 30 ADC data, it makes 40 ADC average data */
       for(quint32 adc_cal_count =0; adc_cal_count<1200; adc_cal_count++)
       {
           adc_average_1st += adc_rawdata[adc_cal_count];

           if(adc_cal_count && (adc_cal_count%30 == 29))
           {
               adc_average.append(adc_average_1st/30);
               adc_average_1st =0;
           }
       }

       /*  2. sorting the adc data & drop the  upper/lower 10 ADC data */
          qSort(adc_average.begin(), adc_average.end());

          adc_average.remove(30, 10);
          adc_average.remove(0, 10);

       /*  3. Calculate the average value of 20 ADC data */
          for(quint8 adc_cal_count=0; adc_cal_count<20; adc_cal_count++)
          {
              adc_average_2nd += adc_average[adc_cal_count];
          }

          adc_average_2nd /= 20;

         Log() <<"Averfage of ADC Value :"<<  QString("%1").arg(adc_average_2nd, 0, 16);

          adc_data_final = static_cast<quint16>(adc_average_2nd);

          read_adc_count = 0;          

          m_timer_adc_transmitt->start();

       /* 4. raw data & average data vector clear */
          adc_rawdata.clear();
          adc_average.clear();
   }
}

void AFEControl::adc_data_transmitt()
{
//  m_ch_ctl_param.m_ch = static_cast<Coding_Channel_Ctl::channel>(conding_ch_number);
//  m_ch_ctl_param.m_resp = Coding_Channel_Ctl::READ_ADC;
    afe_coding_ch_ctl.adc_read_value = adc_data_final;
//  emit sig_cmd_resp(afe_coding_ch_ctl);
    adc_read_start();
}

void AFEControl::dac_stop()
{
    dac_init();
}

void AFEControl::adc_read_stop()
{
    m_timer_adc->stop();
    m_timer_adc_transmitt->stop();
}

void AFEControl::cmd_from_TcpSocket(Coding_Channel_Ctl m_ch_ctl)
{
    qint32 result;
    quint16 *arg = nullptr;

        switch(m_ch_ctl.m_cmd)
        {
           case  Coding_Channel_Ctl::CMD_AFE_READY:

            break;

            case Coding_Channel_Ctl::CMD_START_MEASURE:

                 adc_read_start();

            break;

            case Coding_Channel_Ctl::CMD_STOP_MEASURE:

                 adc_read_stop();

            break;

            case Coding_Channel_Ctl::CMD_DAC_OUT_WORK:

                result = dac_out(AFEControl::DAC_CH::CH_A, static_cast<float>(m_ch_ctl.dac_value_a));

                if(result >0)
                    m_ch_ctl.m_resp = Coding_Channel_Ctl::RESP_DAC_OUT_WORK_SUCCESS;
                else
                    m_ch_ctl.m_resp = Coding_Channel_Ctl::RESP_DAC_OUT_WORK_FAIL;

                break;

            case Coding_Channel_Ctl::CMD_DAC_CHECK_WORK:

                break;

            case Coding_Channel_Ctl::CMD_DAC_OUT_COUNTER:

                result = dac_out(AFEControl::DAC_CH::CH_B, static_cast<float>(m_ch_ctl.dac_value_b));

                if(result >0)
                    m_ch_ctl.m_resp = Coding_Channel_Ctl::RESP_DAC_OUT_COUNTER_SUCCESS;
                else
                    m_ch_ctl.m_resp = Coding_Channel_Ctl::RESP_DAC_OUT_COUNTER_FAIL;

                break;

            case Coding_Channel_Ctl::CMD_DAC_CHECK_COUNTER:

                break;

            case Coding_Channel_Ctl::CMD_FW_CHECK:

                    m_ch_ctl.m_resp = Coding_Channel_Ctl::RESP_FW_CHECK_SUCCESS;

                break;

            case Coding_Channel_Ctl::CMD_BATTERY_CHECK:

                    m_ch_ctl.m_resp = Coding_Channel_Ctl::RESP_BATTERY_CHECK_SUCCESS;

                break;

            case Coding_Channel_Ctl::CMD_UNKNOWN:

                break;

            default:

                break;
        }    

        m_ch_ctl.m_ch = afe_coding_ch_ctl.m_ch;
        emit sig_resp_from_afe(resp_to_json->encode_resp(m_ch_ctl, arg));
}

AFEControl::~AFEControl()
{

}
