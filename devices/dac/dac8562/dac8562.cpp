
#include "dac8562.h"
//#include "../../spi_bridge/ftdi/ft4222.h"
#include "../../spi_bridge/prolific/pl23d3.h"

dac8562::dac8562(QObject *parent) : QObject(parent)
{
//  m_spi_control = new ft4222;
    m_spi_control = new pl23d3;

    initialize();
}

void dac8562::initialize()
{
     DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG);     // reset
     DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B);           // power up
//   DAC_WR_REG(CMD_INTERNAL_REF_EN, DATA_INTERNAL_REF_EN); // enable internal reference
     DAC_WR_REG(CMD_GAIN, DATA_GAIN_B1_A1);                 // set multiplier
     DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS);               // update the caches
}

void dac8562::DAC_WR_REG(quint8 cmd_byte, quint16 data_byte )
{
    quint8 cmd_data[3]={0x0,};

    cmd_data[0] =  cmd_byte;
    cmd_data[1] = (data_byte  >> 8) & 0xff;
    cmd_data[2] =  data_byte & 0xff;

//  m_spi_control->SPI_Single_Write((quint8 *)cmd_data, sizeof(cmd_data));
}

void dac8562::outPutValue(quint8 cmd_byte, quint16 input)
{
    DAC_WR_REG(cmd_byte, input);
}

void dac8562::writeVoltage(float input)
{
    writeA(input);
    writeB(input);
}

void dac8562::writeA(float input)
{
    outPutValue(CMD_SETA_UPDATEA,Voltage_Convert(input));
}

void dac8562::writeB(float input)
{
    outPutValue(CMD_SETB_UPDATEB,Voltage_Convert(input));
}

quint16 dac8562::Voltage_Convert(float voltage)
{
    quint16 _D_;

    float temp_val = 0.0f;
    float Voffset = 0.64f;
    float Vref = 2500.0f;

    temp_val = (voltage - Voffset) / (Vref / 65535.0f);

    _D_ = (quint16)(temp_val + 0.5f);

    return _D_;
}

dac8562::~dac8562()
{

}
