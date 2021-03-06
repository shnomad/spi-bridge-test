#include "dac80004.h"
#include "common.h"
#include "../../spi_bridge/ftdi/ft4222.h"

dac80004::dac80004(QObject *parent) : GpioControl(parent)
{
#if 1
   /*LDAC*/
    GpioControl::exportGPIO(86);
    GpioControl::setDirection(86, GpioControl::SET_DIR::SET_OUT);
    GpioControl::setValue(86, GpioControl::SET_VAL::SET_HIGH);

   /*CLEAR*/
    GpioControl::exportGPIO(84);
    GpioControl::setDirection(84, GpioControl::SET_DIR::SET_OUT);
    GpioControl::setValue(84, GpioControl::SET_VAL::SET_HIGH);

   /*dac8004 H/W clear*/    
    GpioControl::setValue(84, GpioControl::SET_VAL::SET_LOW);
    delay_mSec(100);

    GpioControl::setValue(84, GpioControl::SET_VAL::SET_HIGH);
#endif

    m_spi_control = new ft4222;
}

void dac80004::WriteToBuffer(quint16 data, quint8 channel, bool update)
{
    /*Initialize the cmd buffer*/

#if 1

    quint8 cmd[4]= {0x0,};

   //cmd[4] = data;
   //quint8 temp0, temp1, temp2;

#if 0

    cmd[0] |= (data & 0xF) << 4;
    cmd[1] |= (data & 0x0FF0) >> 4;
    cmd[2] |= (data & 0xF000) >> 12;
    cmd[2] |= 0xF << 4;
    cmd[3] = 0x2;
#else

    cmd[3] |= (data & 0xF) << 4;
    cmd[2] |= (data & 0x0FF0) >> 4;
    cmd[2] |= (data & 0xF000) >> 12;
    cmd[1] |= 0xF << 4;
    cmd[0] = 0x2;

#endif

//  cmd[3] = 0x1 <<4;

#else
    QByteArray cmd;
    cmd.resize(32);

    cmd.fill(0x0);

    /*DAC output value*/
    cmd.insert(4, 0x4, data);

    /*select channel*/
    cmd.insert(20, channel)    ;

    /*Update DAC n*/
    if(update)
    {
        cmd.insert(24, 0x1);
        cmd.insert(25, 0x1);
    }

    /*set R/W bit*/
    cmd.insert(28,0x1);
#endif

    _spi_control->SPI_Single_Write((quint8 *)cmd, sizeof(cmd));

//    GpioControl::setValue(86, GpioControl::SET_VAL::SET_LOW);

//    GpioControl::setValue(86, GpioControl::SET_VAL::SET_HIGH);

}

void dac80004::PowerUpDown(bool UpDown)
{
    quint8 cmd[4]= {0x0,};

    if(UpDown)
    {
        cmd[0] = 0xF;
        cmd[1] = 0x0;
    }

    cmd[3] = 0x1 <<4;

    m_spi_control->SPI_Single_Write((quint8 *)cmd, sizeof(cmd));
}

void dac80004::SoftClear()
{    
   quint8 cmd[4]= {0x0,};

   cmd[3] |= SW_CLEAR;

   m_spi_control->SPI_Single_Write((quint8 *)cmd, sizeof(cmd));
}

void dac80004::SoftReset()
{
   quint8 cmd[4]= {0x0,};

   cmd[3] |= SW_RESET;

   m_spi_control->SPI_Single_Write((quint8 *)cmd, sizeof(cmd));
}

void dac80004::updateDac()
{
    quint8 cmd[4]= {0x0,};

    cmd[2] |= 0xF << 4;
    cmd[3] |= 0x1;

    m_spi_control->SPI_Single_Write((quint8 *)cmd, sizeof(cmd));
}

void dac80004::ReadStatus()
{

}

dac80004::~dac80004()
{

}
