#include "afe_control.h"
#include "gpiosysfs.h"
#include <bits/stdc++.h>
#include <algorithm>

AFEControl::AFEControl(QString hid_port_name, sys_cmd_resp *m_ch_ctl, QObject *parent) : QObject(parent)
{
    resp_to_json = new jsonDataHandle;

    afe_coding_ch_ctl = new sys_cmd_resp;
    afe_coding_ch_ctl = m_ch_ctl;

    m_usb_spi = new pl23d3;
    m_dac = new dac8562;

#ifdef _USE_ADC_ADS1120_
    m_adc = new ads1120;
#endif

#ifdef _USE_ADC_ADS8866_
    m_adc = new ads8866;
#endif

#ifdef _USE_ADC_ADS130E08_
    m_adc = new ads130e08;

    adc_init();
    ads130e08_rdry = new  GpioSysFs(GpioSysFs::GPIO_BANK::BANK_3, 20);

    ads130e08_rdry->gpio_export();
    ads130e08_rdry->gpio_set_dir(GpioSysFs::GPIO_DIR::SET_IN);
    ads130e08_rdry->gpio_set_edge("falling");

    drdy_fd = ads130e08_rdry->gpio_fd_open();
    drdy_notify = new QSocketNotifier(drdy_fd, QSocketNotifier::Exception);
    drdy_notify->setEnabled(false);

    connect(drdy_notify, SIGNAL(activated(int)), this, SLOT(adc_read_ready()));

//  drdy_notify->setEnabled(true);

#endif

    m_timer_adc = new QTimer;
    m_timer_notice = new QTimer;
    m_timer_read_adc_delay = new QTimer; //ADS1120

    QString hidpath = "/dev/"+hid_port_name;

    hid_fd = m_usb_spi->SPI_Open(hidpath.toStdString().c_str());

    if(m_ch_ctl->m_ch == sys_cmd_resp::CH_001)
    {

        if(hid_fd < 0)
        {
            Log()<<m_ch_ctl->m_ch <<" :HID Device Open Fail!!!";
            exit(0);
        }

        m_usb_spi->SPI_Master_Init(hid_fd);
    }

    dac_init();
    adc_init();

#ifdef _USE_ADC_ADS8866_
    /* Socket notifier for SPI read interrupt*/
     m_notify_hid = new QSocketNotifier(hid_fd, QSocketNotifier::Read, this);
     connect(m_notify_hid, SIGNAL(activated(int)), this, SLOT(adc_read_ready()));
     m_notify_hid->setEnabled(true);
#endif

#ifdef _USE_ADC_ADS1120_
    /* Settings for ADS1120, Set DRDY Pin*/
    m_usb_spi->GPIO6B_Set_Direction(hid_fd, pl23d3::GPIO_DIR::GPIO_IN);
    adc_init();    
    connect(this, SIGNAL(sig_read_adc_delay_complete()), this, SLOT(adc_read_ready()));
    /* End of Settings for ADS1120*/
#endif

#ifdef _USE_ADC_ADS130E08_
    m_usb_spi->GPIO6B_Set_Direction(hid_fd, pl23d3::GPIO_DIR::GPIO_OUT);
    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);
//    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_LOW);
//    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);
#endif

    /* read ADC started by timer*/
    m_timer_adc->setSingleShot(true);
    m_timer_adc->setInterval(0);

    QObject::connect(m_timer_adc, SIGNAL(timeout()), this, SLOT(adc_read()));    

    /* notice to websock thread "I'm Ready"*/
    m_timer_notice->setSingleShot(true);
    m_timer_notice->setInterval(100);

    connect(m_timer_notice, &QTimer::timeout,[=]()
          {
//              afe_coding_ch_ctl->m_ch = static_cast<sys_cmd_resp::channel>(conding_ch_number);
                afe_coding_ch_ctl->m_resp = sys_cmd_resp::RESP_AFE_READY_SUCCESS;
//              emit sig_resp_from_afe(afe_coding_ch_ctl);
          });

#ifdef _USE_ADC_ADS1120_
    connect(m_timer_read_adc_delay, &QTimer::timeout,[=]()
          {
                  m_usb_spi->SPI_Single_Read(hid_fd, pl23d3::CS_1);
                  emit sig_read_adc_delay_complete();
          });

    m_timer_read_adc_delay->setSingleShot(true);
    m_timer_read_adc_delay->setInterval(1);

#endif
}

qint32 AFEControl::dac_init()
{
    /* DAC Initialize */

    qint32 result;

    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG), 0x3);

    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B), 0x3);

    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->DAC_WR_REG(CMD_GAIN, DATA_GAIN_B1_A1), 0x3);

    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS), 0x3);

    return result;
}

qint32 AFEControl::dac_out(AFEControl::DAC_CH ch, float value)
{
    /* DAC voltage out */
    qint32 result;

   if(ch == AFEControl::DAC_CH::CH_A)
   {
         result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->writeA(value), 0x3);
         afe_coding_ch_ctl->dac_value_a_v = value;
   }
   else if(ch == AFEControl::DAC_CH::CH_B)
   {
         result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->writeB(value), 0x3);
         afe_coding_ch_ctl->dac_value_b_v = value;
   }
   else
   {
          result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->writeALL(value), 0x3);
   }

   return result;
}

qint32 AFEControl::dac_out(AFEControl::DAC_CH ch, quint16 dac_bit)
{
    /* DAC voltage out */
    qint32 result;

    if(ch == AFEControl::DAC_CH::CH_A)
    {
        result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->writeA(dac_bit), 0x3);
        afe_coding_ch_ctl->dac_value_a_b = dac_bit;
    }
    else if(ch == AFEControl::DAC_CH::CH_B)
    {
        result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_0, m_dac->writeB(dac_bit), 0x3);
        afe_coding_ch_ctl->dac_value_b_b = dac_bit;
    }

    return result;
}

qint32 AFEControl::adc_init()
{
    qint32 result;    

#ifdef _USE_ADC_ADS1120_
    //RESET
     result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->reset(), 0x1);

    //SET GAIN, Conversion Mode
     result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->setConversionMode(0), 0x2);

    //SET Data Rate
     result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->setDataRate(6), 0x2);

    //SET OP Mode
     result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->setOpMode(2), 0x2);

    //SET Reference Voltage
    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->setVoltageRef(1), 0x2);

    //SET DRDY Mode
    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->setDRDYmode(0), 0x2);

    //SELECT ADC Channel 0
    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->setMultiplexer(0x8), 0x2);

#endif

#ifdef _USE_ADC_ADS130E08_

    quint8 read_reg_cmd[2];

    //RESET
    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);
    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_LOW);
    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->reset(), 0x1);
    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);

    //Read Register Test

    read_reg_cmd[0] = 0x20;
    read_reg_cmd[1] = 0x00;

    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_LOW);
    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, read_reg_cmd, 0x2);
    result = m_usb_spi->SPI_Single_Read(hid_fd,pl23d3::CS_1);
    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);


    //STOP RDATAC   
    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_LOW);
    result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->sdatac(), 0x1);
    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);

#endif

    return result;
}

void AFEControl::adc_read_start()
{

    read_adc_count =0;

#ifdef _USE_ADC_ADS8866_
    m_notify_hid->setEnabled(true);
#endif

    m_timer_adc->start();

#ifdef _USE_ADC_ADS1120_
    m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->startSync(), 0x1);       //ADS1120
#endif

}

qint32 AFEControl::adc_read()
{
    qint32 result;

#ifdef _USE_ADC_ADS130E08_
    adc_capture_start=true;
    drdy_notify->setEnabled(true);

    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);
    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_LOW);

    m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->start(), 0x1);

    m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);
#endif

#ifdef _USE_ADC_ADS8866_
    result = m_usb_spi->SPI_Single_Read(hid_fd, pl23d3::CS_1);
#endif

#ifdef _USE_ADC_ADS1120_
   result = m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->startSync(), 0x1);

    while(drdy_pin)
    {
        drdy_pin = m_usb_spi->GPIO6B_Get_Val(hid_fd);
    }

    m_timer_read_adc_delay->start();
#endif
    return result;
}

void AFEControl::adc_read_ready()
{

#ifdef _USE_ADC_ADS130E08_
        ads130e08_rdry->gpio_poll(drdy_fd, 100);
#endif


#ifdef _USE_ADC_ADS130E08_
        drdy_notify->setEnabled(false);

        m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);
        m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_LOW);

        m_usb_spi->SPI_Single_Write(hid_fd, pl23d3::CS_1, m_adc->rdata(), 0x1);
        m_usb_spi->SPI_Single_Read(hid_fd, pl23d3::CS_1);

        m_usb_spi->GPIO6B_Set_OutVal(hid_fd, pl23d3::GPIO_OUT_VAL::GPIO_OUT_HIGH);

        if(adc_capture_start==true)
        {
            adc_data_calculate(10, 600);              //total loop, capture count for every loop
        }
#endif
           adc_data_calculate(10, 600);              //total loop, capture count for every loop        
 //        adc_data_calculate(10, 1200);             //total loop, capture count for every loop
}

void AFEControl::adc_data_calculate(quint32 loop_count, quint32 capture_count)
{

    qint32  result;
    quint16 adc_value = 0;
    quint64 adc_average_1st=0, adc_average_2nd=0;

   if(total_count_loop < loop_count)
    {

       if(read_adc_count < capture_count)
       {

#ifdef _USE_ADC_ADS1120_
           /* Read ADC data and store */
            drdy_pin = 1;
#endif
            result = read(hid_fd, spi_read_buf, 64);                       

            if(spi_read_buf[0] == 0xb3 && spi_read_buf[63] == 0x2)
            {
                adc_value |= spi_read_buf[1] << 8;
                adc_value |= spi_read_buf[2];

#ifdef _USE_ADC_ADS1120_
                if(adc_value == 0)
                {
                    adc_init();
                    goto adc_start_jump;
                }
#endif

                adc_rawdata.append(adc_value);

                read_adc_count++;
            }

        if(read_adc_count == 1 || read_adc_count == capture_count)
        {
            Log()<<"["<< read_adc_count <<"]"<< " Read ADC Value :"<<  QString("%1").arg(adc_value, 0, 16);
            Log()<< "Voltage :"<< (3000.f/65535.0f)*adc_value<<"mV";
        }

#ifdef _USE_ADC_ADS1120_
            Log()<<"voltage value :" << (3248/32767.0f)*adc_value <<"mV";
#endif
            adc_value = 0;

#ifdef _USE_ADC_ADS1120_
            adc_start_jump:
#endif
            m_timer_adc->start();
       }
       else
       {

#ifdef _USE_ADC_ADS8866_
           m_notify_hid->setEnabled(false);
#endif
            /* ADC min. max. value check */
            if(total_count_loop == 0)
            {
                adc_min = *min_element(adc_rawdata.begin(), adc_rawdata.end());
                adc_max = *max_element(adc_rawdata.begin(), adc_rawdata.end());
            }
            else
            {
                quint16 tmp_min, tmp_max;

                tmp_min = *min_element(adc_rawdata.begin(), adc_rawdata.end());
                tmp_max = *max_element(adc_rawdata.begin(), adc_rawdata.end());

                if(adc_min > tmp_min)
                {
                    adc_min = adc_min - tmp_min;
                    tmp_min = adc_min + tmp_min;
                    adc_min = tmp_min - adc_min;
                }

                if(adc_max < tmp_max)
                {
                    tmp_max = tmp_max - adc_max;
                    adc_max = tmp_max + adc_max;
                    tmp_max = adc_max - tmp_max;
                }
            }

           /* 1. Calculate the average value of every 30 ADC data, it makes ADC average data */

           for(quint32 adc_cal_count =0; adc_cal_count<capture_count; adc_cal_count++)
            {
               adc_average_1st += adc_rawdata[adc_cal_count];

               if(adc_cal_count && (adc_cal_count%30 == 29))
               {
                   adc_average.append(adc_average_1st/30);
                   adc_average_1st =0;
               }
           }

           /*  2. sorting the adc data & drop the  upper/lower ADC data */
              qSort(adc_average.begin(), adc_average.end());

              quint16 quater_count = 0;

              quater_count = (quint16)adc_average.size()/4;

              adc_average.remove(adc_average.size()-quater_count, quater_count);
              adc_average.remove(0, quater_count);

           /*  3. Calculate the average value of ADC data */
              for(quint8 adc_cal_count=0; adc_cal_count<adc_average.size(); adc_cal_count++)
              {
                  adc_average_2nd += adc_average[adc_cal_count];
              }

              adc_average_2nd /= adc_average.size();

             //Log() <<"Averfage of ADC Value :"<<  QString("%1").arg(adc_average_2nd, 0, 16);

              adc_data_final = static_cast<quint16>(adc_average_2nd);
              read_adc_count = 0;

           /* 4. raw data & average data vector clear */
              adc_rawdata.clear();
              adc_average.clear();

              Log() <<"Final ADC Value :"<<  QString("%1").arg(adc_data_final, 0, 16);

              adc_data_resp[total_count_loop] = adc_data_final;

              adc_data_final = 0;

              if(total_count_loop < 9)
              {
                  total_count_loop++;
                  adc_read_start();
              }
              else
              {
                  adc_data_resp[10] = adc_max;
                  adc_data_resp[11] = adc_min;

                  afe_coding_ch_ctl->m_resp = sys_cmd_resp::RESP_MEASURED_ADC_VALUE;

                  emit sig_resp_from_afe(resp_to_json->encode_resp(afe_coding_ch_ctl, adc_data_resp));

                  memset((void *)adc_data_resp, 0x0, sizeof(adc_data_resp));

                  total_count_loop = 0;
              }
       }
    }

}

void AFEControl::dac_stop()
{
    dac_init();
}

void AFEControl::adc_read_stop()
{
    m_timer_adc->stop();

    memset((void *)adc_data_resp, 0x0, sizeof(adc_data_resp));

    adc_rawdata.clear();
    adc_average.clear();

}

void AFEControl::cmd_from_TcpSocket(sys_cmd_resp *cmd_from_host)
{
    qint32 result;
    quint16 *arg1, *arg2 = nullptr;

    switch(cmd_from_host->m_cmd)
    {
        case sys_cmd_resp::CMD_AFE_READY:

            resp_to_json->mutex.lock();
                cmd_from_host = resp_to_json->check_config(cmd_from_host);
            resp_to_json->mutex.unlock();

        break;

        case sys_cmd_resp::CMD_START_MEASURE:

            resp_to_json->mutex.lock();
                cmd_from_host = resp_to_json->check_config(cmd_from_host);
            resp_to_json->mutex.unlock();

            if(cmd_from_host->m_resp == sys_cmd_resp::RESP_AFE_READY_FAIL)
                break;

            /*DAC out WOKR_BIT*/
            result = dac_out(AFEControl::DAC_CH::CH_A, static_cast<quint16>(cmd_from_host->resp_arg[2]));

            if(result<=0)
            {
                cmd_from_host->m_resp = sys_cmd_resp::RESP_START_MEASURE_FAIL;
                cmd_from_host->resp_arg[0] = sys_cmd_resp::RESP_DAC_OUT_WORK_FAIL;
                break;
            }

            /*DAC out COUNTER_BIT*/
            result = dac_out(AFEControl::DAC_CH::CH_B, static_cast<quint16>(cmd_from_host->resp_arg[3]));

            if(result<=0)
            {
                cmd_from_host->m_resp = sys_cmd_resp::RESP_START_MEASURE_FAIL;
                cmd_from_host->resp_arg[0] = sys_cmd_resp::RESP_DAC_OUT_COUNTER_FAIL;
                break;
            }

            adc_read_start();
            cmd_from_host->m_resp = sys_cmd_resp::RESP_START_MEASURE_SUCCESS;

        break;

        case sys_cmd_resp::CMD_STOP_MEASURE:

            adc_read_stop();
            cmd_from_host->m_resp = sys_cmd_resp::RESP_STOP_MEASURE_SUCCESS;

        break;

        case sys_cmd_resp::CMD_DAC_OUT_WORK_VOLT:
            {

                result = dac_out(AFEControl::DAC_CH::CH_A, static_cast<float>(cmd_from_host->dac_value_a_v));

                if(result >0)
                {
                    cmd_from_host->m_resp = sys_cmd_resp::RESP_DAC_OUT_WORK_SUCCESS;
                }
                else
                {
                    cmd_from_host->m_resp = sys_cmd_resp::RESP_DAC_OUT_WORK_FAIL;
                }
            }
        break;

        case sys_cmd_resp::CMD_DAC_OUT_COUNTER_VOLT:
            {

                result = dac_out(AFEControl::DAC_CH::CH_B, static_cast<float>(cmd_from_host->dac_value_b_v));

                if(result >0)
                {
                    cmd_from_host->m_resp = sys_cmd_resp::RESP_DAC_OUT_COUNTER_SUCCESS;
                }
                else
                {
                    cmd_from_host->m_resp = sys_cmd_resp::RESP_DAC_OUT_COUNTER_FAIL;
                }
            }
        break;

        case sys_cmd_resp::CMD_DAC_OUT_WORK_BIT:
            {
                result = dac_out(AFEControl::DAC_CH::CH_A, static_cast<quint16>(cmd_from_host->dac_value_a_b));

                if(result >0)
                {
                    cmd_from_host->m_resp = sys_cmd_resp::RESP_DAC_OUT_WORK_SUCCESS;
                }
                else
                {
                    cmd_from_host->m_resp = sys_cmd_resp::RESP_DAC_OUT_WORK_FAIL;
                }
            }
        break;

        case sys_cmd_resp::CMD_DAC_OUT_COUNTER_BIT:
            {
                result = dac_out(AFEControl::DAC_CH::CH_B, static_cast<quint16>(cmd_from_host->dac_value_b_b));

                if(result >0)
                {
                    cmd_from_host->m_resp = sys_cmd_resp::RESP_DAC_OUT_COUNTER_SUCCESS;
                }
                else
                {
                    cmd_from_host->m_resp = sys_cmd_resp::RESP_DAC_OUT_COUNTER_FAIL;
                }
            }
        break;

        case sys_cmd_resp::CMD_DAC_VOLT_CHECK:

        break;

        case sys_cmd_resp::CMD_DAC_BIT_CHECK:

        break;

        case sys_cmd_resp::CMD_UNKNOWN:
        break;
    }

    resp_to_json->mutex.lock();

        emit sig_resp_from_afe(resp_to_json->encode_resp(cmd_from_host, arg2));

    resp_to_json->mutex.unlock();

}

AFEControl::~AFEControl()
{

}
