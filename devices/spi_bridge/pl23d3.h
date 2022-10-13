#ifndef PL23D3_H
#define PL23D3_H

#include <QObject>
#include <QSocketNotifier>
#include "common.h"

/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class pl23d3 : public QObject
{
    Q_OBJECT

public:
    enum CHIP_SELECT
    {
      CS_0=0x03,
      CS_1=0x13
    };

    enum GPIO_PIN
    {
        GPA4=0x0,
        GPA5,
        GPA6,
        GPB6
    };

    enum GPIO_DIR
    {
        GPIO_IN = 0x0,
        GPIO_OUT
    };

    enum GPIO_OUT_VAL
    {
        GPIO_OUT_LOW = 0x0,
        GPIO_OUT_HIGH
    };

    enum GPIO_IN_VAL
    {
        GPIO_IN_LOW =0x0,
        GPIO_IN_HIGH
    };

    explicit pl23d3(QObject *parent = nullptr);
        ~pl23d3();
    void SPI_Bridge_Info();
    void SPI_Open(quint16, quint16);
    int SPI_Open(const char *);
    void SPI_Close(int);
    qint32 SPI_Master_Init(int);
    qint32 SPI_Single_Write(int, CHIP_SELECT, quint8 *, quint8);
    qint32 SPI_Single_Read(int, CHIP_SELECT);
    void  GPIO6B_Set_Direction(int, GPIO_DIR);
    void  GPIO6B_Set_OutVal(int, GPIO_OUT_VAL);
    quint8 GPIO6B_Get_Val(int);

signals:

public slots:
    qint32 SPI_Ready_read(int);

    private:
    quint8 feature_report_buf[6] ={0x0,}, spi_write_buf[64]= {0xff,}, spi_read_buf[64]= {0x0,}, read_adc[4] = {0x0,};
    quint8 gpio_report_buf[64]={0x0,};

//  int fd;
//  QSocketNotifier *m_notify_hid, *m_notify_error;
};

#endif // PL23D3_H
