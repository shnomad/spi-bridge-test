#include "pl23d3.h"

pl23d3::pl23d3(QObject *parent) : QObject(parent)
{

}

void  pl23d3::SPI_Open(quint16 VID, quint16 PID)
{

}

int pl23d3::SPI_Open(const char *path)
{
  int fd = open(path, O_RDWR|O_NONBLOCK);

    if(fd < 0)
        qDebug()<<"USB device Open Error";

    return fd;
}

void pl23d3::SPI_Close(int fd)
{
    close(fd);
}

void pl23d3::SPI_Bridge_Info()
{
#if 0
    wchar_t wstr[255];

    hid_get_manufacturer_string(handle, wstr, 255);

    qDebug()<< QString::fromWCharArray(wstr);

    hid_get_product_string(handle, wstr, 255);

    qDebug()<< QString::fromWCharArray(wstr);

    hid_get_serial_number_string(handle, wstr, 255);

    qDebug()<< QString::fromWCharArray(wstr);

    hid_get_indexed_string(handle, 1, wstr, 255);

    qDebug()<< QString::fromWCharArray(wstr);
#endif
}

qint32 pl23d3::SPI_Master_Init(int fd)
{
    /***  MODE = 1  ***/
    qint32 result=0;

    feature_report_buf[0] = 0x81;
    feature_report_buf[1] = 0x0f;

    result = ioctl(fd, HIDIOCGFEATURE(2), feature_report_buf);

    memset(feature_report_buf,0x00,sizeof(feature_report_buf));

    feature_report_buf[0] = 0x80;
    feature_report_buf[1] = 0x0f;
    feature_report_buf[2] = 0x10;

    result = ioctl(fd, HIDIOCSFEATURE(3), feature_report_buf);

    memset(feature_report_buf,0x00,sizeof(feature_report_buf));

    feature_report_buf[0] = 0x80;
    feature_report_buf[1] = 0x10;
    //   feature_report_buf[2] = 0x1d;       /* SPI Frequency(KHz) = 24,000/(29+1) = 800 KHz */
    //   feature_report_buf[2] = 0x13;       /* SPI Frequency(KHz) = 24,000/(19+1) = 1,200 KHz */
    feature_report_buf[2] = 0x3;        /* SPI Frequency(KHz) = 24,000/(3+1) =  6,000 KHz */

    result = ioctl(fd, HIDIOCSFEATURE(3), feature_report_buf);

    memset(feature_report_buf,0x00,sizeof(feature_report_buf));

    return result;
}

qint32 pl23d3::SPI_Single_Write(int fd, CHIP_SELECT chip_sel ,quint8 *cmd_buf, quint8 size)
{
    /** SPI Write with CS **/

    qint32 result;

    spi_write_buf[0] = 0xb2;
    spi_write_buf[1] = chip_sel;
    spi_write_buf[2] = size;

    for(quint8 cmd_count=0; cmd_count<size; cmd_count++)
        spi_write_buf[cmd_count+3] = cmd_buf[cmd_count];

    result = write(fd, spi_write_buf, sizeof(spi_write_buf));

    memset(spi_write_buf,0x00,sizeof(spi_write_buf));

    return result;
}

qint32 pl23d3::SPI_Single_Read(int fd, CHIP_SELECT chip_sel)
{
    /** SPI Read with CS1, ADC Data capture **/
    qint32 result;    

    memset(read_adc,0x00,sizeof(read_adc));
    memset(spi_write_buf,0xff,sizeof(spi_write_buf));

    spi_write_buf[0] = 0xb3;
    spi_write_buf[1] = chip_sel;
    spi_write_buf[2] = 0x02;
    spi_write_buf[3] = 0x00;

    result = write(fd, spi_write_buf, 64);

    memset(spi_write_buf,0x00,sizeof(spi_write_buf));

    return result;
}

qint32 pl23d3::SPI_Ready_read(int fd)
{
    qint32 result;
    quint16 adc_value;

    result = read(fd, spi_read_buf, 64);

    if(spi_read_buf[0] == 0xb3 && spi_read_buf[63] == 0x2)
    {
        adc_value |= spi_read_buf[1] << 8;
        adc_value |= spi_read_buf[2];
    }

    qDebug()<<"Read ADC Value :"<<  QString("%1").arg(adc_value, 0, 16);

    return result;
}
void  pl23d3::GPIO6B_Set_Direction(int fd, GPIO_DIR dir)
{
        qint32 result=0;

        /*Direction Setting Start  */

        /***/
        memset(feature_report_buf,0x00,sizeof(feature_report_buf));

        feature_report_buf[0] = 0x81;
        feature_report_buf[1] = 0x03;

        result = ioctl(fd, HIDIOCSFEATURE(2), feature_report_buf);      //1

        /***/
        memset(feature_report_buf,0x00,sizeof(feature_report_buf));

        feature_report_buf[0] = 0x81;
        feature_report_buf[1] = 0x03;

        result = ioctl(fd, HIDIOCGFEATURE(2), feature_report_buf);      //2

        /***/
        memset(feature_report_buf,0x00,sizeof(feature_report_buf));

        feature_report_buf[0] = 0x80;
        feature_report_buf[1] = 0x03;

        if(dir == GPIO_DIR::GPIO_OUT)
            feature_report_buf[2] = 0x40;
        else
            feature_report_buf[2] = 0x00;

        result = ioctl(fd, HIDIOCSFEATURE(3), feature_report_buf);      //3

        /***/
        memset(feature_report_buf,0x00,sizeof(feature_report_buf));

        feature_report_buf[0] = 0x81;
        feature_report_buf[1] = 0x05;

        result = ioctl(fd, HIDIOCSFEATURE(2), feature_report_buf);      //4

        /***/
        memset(feature_report_buf,0x00,sizeof(feature_report_buf));

        feature_report_buf[0] = 0x81;
        feature_report_buf[1] = 0x03;

        result = ioctl(fd, HIDIOCGFEATURE(2), feature_report_buf);      //5

        /***/
        memset(feature_report_buf,0x00,sizeof(feature_report_buf));

        feature_report_buf[0] = 0x80;
        feature_report_buf[1] = 0x05;
        feature_report_buf[2] = 0x40;

        result = ioctl(fd, HIDIOCSFEATURE(3), feature_report_buf);      //6
}

void  pl23d3::GPIO6B_Set_OutVal(int fd, GPIO_OUT_VAL value)
{
     qint32 result=0;

    feature_report_buf[0] = 0x81;    
    feature_report_buf[1] = 0x01;

    result = ioctl(fd, HIDIOCSFEATURE(2), feature_report_buf);

    feature_report_buf[0] = 0x81;
    feature_report_buf[1] = 0x01;

    result = ioctl(fd, HIDIOCGFEATURE(2), feature_report_buf);

    feature_report_buf[0] = 0x80;
    feature_report_buf[1] = 0x01;

    if(value == GPIO_OUT_VAL::GPIO_OUT_HIGH)
        feature_report_buf[2] = 0xff;
    else
        feature_report_buf[2] = 0xbf;

    result = ioctl(fd, HIDIOCSFEATURE(3), feature_report_buf);

    memset(feature_report_buf,0x00,sizeof(feature_report_buf));
}

quint8 pl23d3::GPIO6B_Get_Val(int fd)
{
    qint32 result=0;
    quint8 value=0;

    memset(feature_report_buf,0x00,sizeof(feature_report_buf));

    feature_report_buf[0] = 0x81;
    feature_report_buf[1] = 0x01;

    result = ioctl(fd, HIDIOCSFEATURE(2), feature_report_buf);

    memset(feature_report_buf,0x00,sizeof(feature_report_buf));

    feature_report_buf[0] = 0x81;
    feature_report_buf[1] = 0x03;

    result = ioctl(fd, HIDIOCGFEATURE(2), feature_report_buf);      //6
    value = (feature_report_buf[1]&0x40) >> 6;

    return value;
}

pl23d3::~pl23d3()
{

}
