#include "codingchannel.h"
#include <bits/stdc++.h>

CodingChannel::CodingChannel(quint8 port, QObject *parent) : QObject(parent)
{    
    QList<QString> hid_port_name = {"usb-spi-1","usb-spi-2","usb-spi-3","usb-spi-4","usb-spi-5", \
                                    "usb-spi-6","usb-spi-7","usb-spi-8","usb-spi-9","usb-spi-10", \
                                    "usb-spi-11","usb-spi-12","usb-spi-13","usb-spi-14","usb-spi-15"};

    m_usb_spi = new pl23d3;
    m_dac = new dac8562;
    m_adc = new ads8866;

    QString hidpath = "/dev/hidraw0";
//  QString hidpath = hid_port_name.at(port);

    hid_fd = m_usb_spi->SPI_Open(hidpath.toStdString().c_str());

    if(hid_fd < 0)
    {
        Log()<<"HID Device Open Fail";
        exit(0);
    }

    m_usb_spi->SPI_Master_Init(hid_fd);

    dac_init();

    qint32 result = dac_out(CodingChannel::DAC_CH::CH_A, 300.0f);
           result = dac_out(CodingChannel::DAC_CH::CH_B, 300.0f);

    if(result< 0)
        exit(0);

    /* Socket notifier for SPI read interrupt*/
    m_notify_hid = new QSocketNotifier(hid_fd, QSocketNotifier::Read, this);

    connect(m_notify_hid, SIGNAL(activated(int)), this, SLOT(adc_read_ready()));

    m_notify_hid->setEnabled(true);

    /* ADC data start timer, start immediately */
    m_timer_adc.setSingleShot(true);
    m_timer_adc.setInterval(1);

    QObject::connect(&m_timer_adc, SIGNAL(timeout()), this, SLOT(adc_read()));

    /* ADC data transmittion timer, after ADC capure & calculate complete it'll start */
    m_timer_adc_transmitt.setSingleShot(true);
    m_timer_adc_transmitt.setInterval(5000);    

    QObject::connect(&m_timer_adc_transmitt, SIGNAL(timeout()), this, SLOT(adc_data_transmitt()));

    /*Later, it'll controlled by Host/server */
 //  adc_read_start();
}

qint32 CodingChannel::dac_init()
{
    /* DAC Initialize */

    qint32 result;

    result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG));

    result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B));

    result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->DAC_WR_REG(CMD_GAIN, DATA_GAIN_B1_A1));

    result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS));

    return result;
}

qint32 CodingChannel::dac_out(CodingChannel::DAC_CH ch, float value)
{
    /* DAC voltage out */

    qint32 result;

   if(ch == CodingChannel::DAC_CH::CH_A)
     result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->writeA(value));
   else if(ch == CodingChannel::DAC_CH::CH_B)
     result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->writeB(value));
   else
     result = m_usb_spi->SPI_Single_Write(hid_fd, m_dac->writeALL(value));

   return result;
}

void CodingChannel::adc_read_start()
{
    m_notify_hid->setEnabled(true);
    m_timer_adc.start();
}

qint32 CodingChannel::adc_read()
{
    qint32 result;
    result = m_usb_spi->SPI_Single_Read(hid_fd);

    return result;
}

void CodingChannel::adc_read_ready()
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

        if(read_adc_count ==1 || read_adc_count == 1200)
            Log()<<"["<< read_adc_count <<"]"<< " Read ADC Value :"<<  QString("%1").arg(adc_value, 0, 16);

        adc_value = 0;

        m_timer_adc.start();
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

          m_timer_adc_transmitt.start();

       /* 4. raw data & average data vector clear */
          adc_rawdata.clear();
          adc_average.clear();
   }
}

void CodingChannel::adc_data_transmitt()
{
    Log();
    emit sig_transmitt_adc("adc_data_final");
    adc_read_start();
}

void CodingChannel::dac_stop()
{

}

void CodingChannel::adc_read_stop()
{
    m_timer_adc.stop();
    m_timer_adc_transmitt.stop();
}

CodingChannel::~CodingChannel()
{

}
