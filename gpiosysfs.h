#ifndef GPIOSYSFS_H
#define GPIOSYSFS_H

#include <QObject>

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 256

class GpioSysFs : public QObject
{
    Q_OBJECT
public:
    enum GPIO_BANK{
        BANK_1 = 0x1,
        BANK_2 = 0x2,
        BANK_3 = 0x3,
        BANK_4 = 0x4
    };

    enum GPIO_DIR{
        SET_IN = 0x0,
        SET_OUT
    };

    enum GPIO_VAL{
        SET_LOW = 0x0,
        SET_HIGH
    };

    explicit GpioSysFs(GPIO_BANK,int, QObject *parent = nullptr);
    int gpio_export();
    int gpio_unexport();
    int gpio_set_dir(GPIO_DIR);
    int gpio_set_value(GPIO_VAL);
    int gpio_get_value(unsigned int &value);
    int gpio_set_edge(const char *edge);
    int gpio_fd_open();
    int gpio_poll(int gpio_fd, int timeout);

signals:

public slots:

private:
    int gpio;
};

#endif // GPIOSYSFS_H
