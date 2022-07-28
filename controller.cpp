#include <QtDebug>
#include "controller.h"
#include "devices/spi_bridge/prolific/pl23d3.h"

Controller::Controller(QObject *parent) : QObject(parent)
{
    m_usb_spi = new pl23d3;
    m_dac = new dac8562;
    m_adc = new ads8866;

    qint32 result=0;
    qint32 usb_hid_fd;
    volatile float voltage;

    QString hidpath = "/dev/coding_ch_1";

    usb_hid_fd = m_usb_spi->SPI_Open("/dev/coding_ch_1");

    if(usb_hid_fd < 0)
    {
        qDebug()<<"HID Device Open Fail";
        exit(0);
    }

    result = m_usb_spi->SPI_Master_Init(usb_hid_fd);

    /* DAC Initialize */
    result = m_usb_spi->SPI_Single_Write(usb_hid_fd, m_dac->DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG));

    result = m_usb_spi->SPI_Single_Write(usb_hid_fd, m_dac->DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B));

    result = m_usb_spi->SPI_Single_Write(usb_hid_fd, m_dac->DAC_WR_REG(CMD_GAIN, DATA_GAIN_B1_A1));

    result = m_usb_spi->SPI_Single_Write(usb_hid_fd, m_dac->DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS));

    /* DAC voltage out */
    result = m_usb_spi->SPI_Single_Write(usb_hid_fd, m_dac->writeA(500.0f));

    result = m_usb_spi->SPI_Single_Write(usb_hid_fd, m_dac->writeB(500.0f));

    /*ADC data read*/
    result = m_usb_spi->SPI_Single_Read(usb_hid_fd, read_adc);

    voltage = m_adc->CalAdcValue(read_adc);

    qDebug()<<"End of Test";
}

void Controller::ReadyRead()
{

}

Controller::~Controller()
{

}
