#include "dac8562.h"


//dac8562::dac8562(int fd, QObject *parent) : QObject(parent)
dac8562::dac8562(QObject *parent) : QObject(parent)
{      
 //   initialize();
}

void dac8562::initialize()
{
     DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG);                 // reset
     DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B);                       // power up
//   DAC_WR_REG(fthandle,CMD_INTERNAL_REF_EN, DATA_INTERNAL_REF_EN);    // enable internal reference
     DAC_WR_REG(CMD_GAIN, DATA_GAIN_B1_A1);                             // set multiplier
     DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS);                           // update the caches
}

quint8 *dac8562::DAC_WR_REG(quint8 cmd_byte, quint16 data_byte)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    cmd_buffer[0] =  cmd_byte;
    cmd_buffer[1] =  (data_byte >> 8) & 0xff;
    cmd_buffer[2] =  data_byte & 0xff;

    return cmd_buffer;
}

quint8 *dac8562::outPutValue(quint8 cmd_byte, quint16 input)
{
    return DAC_WR_REG(cmd_byte, input);
}

quint8 *dac8562::writeA(float input)
{
    return outPutValue(CMD_SETA_UPDATEA,Voltage_Convert(input));
}

quint8 *dac8562::writeB(float input)
{
    return outPutValue(CMD_SETB_UPDATEB,Voltage_Convert(input));
}

quint8 *dac8562::writeALL(float input)
{
    return outPutValue(CMD_UPDATE_ALL_DACS,Voltage_Convert(input));
}

quint8 * dac8562::writeA(quint16 input)
{
    return outPutValue(CMD_SETA_UPDATEA,input);
}

quint8 * dac8562::writeB(quint16 input)
{
    return outPutValue(CMD_SETB_UPDATEB,input);
}

quint16 dac8562::Voltage_Convert(float voltage)
{
    quint16 _D_;

    float temp_val = 0.0f;
    float Voffset = 0.64f;
    float Vref = 3000.0f;

    temp_val = (voltage - Voffset) / (Vref / 65535.0f);

    _D_ = (quint16)(temp_val + 0.5f);

    return _D_;
}

dac8562::~dac8562()
{

}
