#ifndef SH1106_H
#define SH1106_H

#include <QObject>
#include "gpiocontrol.h"

class ft4222;

class sh1106 : public GpioControl
{
    Q_OBJECT
public:
    explicit sh1106(QObject *parent = nullptr);
    ~sh1106();

    enum CMD_DATA_SEL {
        CMD = 0x0,
        DATA
    };

    enum CS_LEVEL {
        LOW = 0x0,
        HIGH
    };

    void Reset(void);
    void HW_Init(void);
    bool Write_Byte(CMD_DATA_SEL, quint8 *, quint16);
    void Display_Init(void);
//    void Display_On(void);
//    void Display_Off(void);
    void Clear_Screen(quint8);

   ft4222 *m_spi_control;

signals:

public slots:

private:
//    ft4222 *m_spi_control;
};

#endif // SH1106_H
