#include "ds18b20.h"
#include "common.h"

ds18b20::ds18b20()
{
    m_gpio.exportGPIO(84);
}

void ds18b20::operation()
{
    if (presence(84) == 1)
    {
        qDebug()<<"No device \n";
    } else
    {
        qDebug()<<"Device present \n";
    }
    fflush(stdout);

    float t;

    for (;;)
    {
        do
        {
            t = getTemperature(84);
        } while (t<-999);
        qDebug()<<"temperature :"<<t;

        fflush(stdout);
    };
#if 0
    bcm2835_close();
#endif
}

int ds18b20::presence(quint8 pin)
{
#if 1

    bool pin_value;

    m_gpio.setDirection(pin, GpioControl::SET_OUT);
    m_gpio.setValue(pin, GpioControl::SET_LOW);
    m_gpio.setValue(pin, GpioControl::SET_HIGH);
//    usleep(480);
     delay_mSec(480);
    m_gpio.setDirection(pin, GpioControl::SET_IN);
//    usleep(70);
    delay_mSec(70);
    m_gpio.readValue(pin, pin_value);
//    usleep(410);
    delay_mSec(410);

#else
     bcm2835_gpio_fsel(pin ,BCM2835_GPIO_FSEL_OUTP);
     bcm2835_gpio_write(pin,  LOW);
     bcm2835_delayMicroseconds(480);
     bcm2835_gpio_fsel(pin,  BCM2835_GPIO_FSEL_INPT);
     bcm2835_delayMicroseconds(70);
     uint8_t b = bcm2835_gpio_lev(pin);
     bcm2835_delayMicroseconds(410);

#endif

     return pin_value;
}

void ds18b20::writeBit(quint8 pin,  quint8 b)
{
    int delay1,  delay2;

    if (b == 1)
    {
        delay1 = 6;
        delay2 = 64;
    } else
    {
        delay1 = 60;
        delay2 = 10;
    }
#if 1
    m_gpio.setDirection(pin, GpioControl::SET_OUT);
    m_gpio.setValue(pin, GpioControl::SET_LOW);
      delay_mSec(delay1);
//    usleep(delay1);
    m_gpio.setDirection(pin, GpioControl::SET_IN);
    delay_mSec(delay2);
//    usleep(delay2);
#else
    bcm2835_gpio_fsel(pin,  BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin,  LOW);
    bcm2835_delayMicroseconds(delay1);
    bcm2835_gpio_fsel(pin,  BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(delay2);
#endif
}

void ds18b20::writeByte(quint8 pin,  quint8 byte)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        if (byte & 1)
        {
            writeBit(pin,  1);
        } else
        {
            writeBit(pin,  0);
        }

        byte = byte >> 1;
    }
}

quint8 ds18b20::readBit(quint8 pin)
{
#if 1

    bool pin_value;

    m_gpio.setDirection(pin, GpioControl::SET_OUT);
    m_gpio.setValue(pin, GpioControl::SET_LOW);
//    usleep(8);
    delay_mSec(1);
    m_gpio.setDirection(pin, GpioControl::SET_IN);
//    usleep(2);
    delay_mSec(1);
    m_gpio.readValue(pin, pin_value);
//    usleep(60);
    delay_mSec(1);

#else
    bcm2835_gpio_fsel(pin,  BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin,  LOW);
    bcm2835_delayMicroseconds(8);
    bcm2835_gpio_fsel(pin,  BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(2);
    uint8_t b = bcm2835_gpio_lev(pin);
    bcm2835_delayMicroseconds(60);

#endif
    return pin_value;
}

int ds18b20::convert(quint8 pin)
{
    int i;
     writeByte(pin,  0x44);

     for (i = 0; i < 5000; i++)
     {
//         usleep(100000);
         delay_mSec(1);
         if (readBit(pin) == 1)
             break;
     }
     return i;
}

float ds18b20::getTemperature(quint8 pin)
{
    if (presence(pin) == 1)
        return -1000;
    writeByte(pin,  0xCC);

    if (convert(pin) == 5000)
        return -3000;
    presence(pin);
    writeByte(pin,0xCC);
    writeByte(pin,0xBE);
    int i;
    uint8_t data[9];
    for (i = 0; i < 9; i++) {
        data[i] = readByte(pin);
    }
    uint8_t crc = crc8(data,  9);
    if (crc != 0) return -2000;
    int t1 = data[0];
    int t2 = data[1];
    int16_t temp1 = (t2 << 8 | t1);
    float temp = (float) temp1 / 16;
    return temp;
}

int ds18b20::readByte(quint8 pin)
{
    int byte = 0;
    int i;
    for (i = 0; i < 8; i++)
    {
        byte = byte | readBit(pin) << i;
    };
    return byte;
}

quint8 ds18b20::crc8(quint8 *data,  quint8 len)
{
    uint8_t i;
    uint8_t j;
    uint8_t temp;
    uint8_t databyte;
    uint8_t crc = 0;

    for (i = 0; i < len; i++)
    {
        databyte = data[i];
        for (j = 0; j < 8; j++) {
            temp = (crc ^ databyte) & 0x01;
            crc >>= 1;
            if (temp)
                crc ^= 0x8C;
            databyte >>= 1;
        }
    }
    return crc;
}
