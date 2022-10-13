#ifndef DS18B20_H
#define DS18B20_H

#include <QObject>
#include <QThread>
#include "gpiocontrol.h"

class ds18b20
{
public:
    ds18b20();
    void operation();
    int presence(quint8 pin);
    void writeBit(quint8 pin,  quint8 b);
    void writeByte(quint8 pin,  quint8 byte);
    quint8 readBit(quint8 pin);
    int convert(quint8 pin);
    float getTemperature(quint8 pin);
    int readByte(quint8 pin);
    quint8 crc8(quint8 *data,  quint8 len);

private:
    GpioControl m_gpio;
};

#endif // DS18B20_H
