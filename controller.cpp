#include <QtDebug>
#include "controller.h"
#include "devices/spi_bridge/prolific/pl23d3.h"

Controller::Controller(QObject *parent) : QObject(parent)
{
    m_usb_spi = new pl23d3;
    m_dac = new dac8562;
    m_adc = new ads8866;

    qint32 result=0;    
    volatile float voltage;

    fd_hid = m_usb_spi->SPI_Open("/dev/hidraw0");

    if(fd_hid < 0)
    {
        qDebug()<<"HID Device Open Fail";
        exit(0);
    }

    result = m_usb_spi->SPI_Master_Init(fd_hid);

    /*ADC data read*/
//  result = m_usb_spi->SPI_Single_Read(usb_hid_fd, read_adc);

    voltage = m_adc->CalAdcValue(read_adc);

    qDebug()<<"End of Test";
}

qint32 Controller::dac_init()
{
     qint32 result=0;

    /* DAC Initialize */
    result = m_usb_spi->SPI_Single_Write(fd_hid, m_dac->DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG));

    result = m_usb_spi->SPI_Single_Write(fd_hid, m_dac->DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B));

    result = m_usb_spi->SPI_Single_Write(fd_hid, m_dac->DAC_WR_REG(CMD_GAIN, DATA_GAIN_B1_A1));

    result = m_usb_spi->SPI_Single_Write(fd_hid, m_dac->DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS));
}

void Controller::adc_read_start()
{

}

qint32 Controller::dac_out(Controller::DAC_CH ch, float value)
{

}

qint32 Controller::adc_read()
{

}

void Controller::adc_read_ready()
{

}

void Controller::adc_data_transmitt()
{

}

void Controller::dac_stop()
{

}

void Controller::adc_read_stop()
{

}

Controller::~Controller()
{

}
