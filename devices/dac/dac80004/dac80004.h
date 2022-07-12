#ifndef DAC80004_H
#define DAC80004_H

#include <QObject>
#include "gpiocontrol.h"

class ft4222;

class dac80004 : public GpioControl
{
    Q_OBJECT
public:

    enum DAC_CHANNEL
    {
        CH_A = 0x0,
        CH_B = 0x1,
        CH_C = 0x2,
        CH_D = 0x3,
        CH_ALL = 0xF
    };

    enum POWER_DOWN
    {
        NORMAL = 0x0,
        _1K_OHM = 0x1,
        _100K_OHM = 0x2,
        _HIZ = 0x3
    };

    explicit dac80004(QObject *parent = nullptr);
    ~dac80004();
    void WriteToBuffer(quint16, quint8, bool);
    void PowerUpDown(bool);
    void SoftClear();
    void ReadStatus();

    ft4222 *m_spi_control;
signals:

public slots:

};

#endif // DAC80004_H
