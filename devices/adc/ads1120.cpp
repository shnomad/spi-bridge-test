#include "ads1120.h"

ads1120::ads1120(QObject *parent) : QObject(parent)
{

}

void ads1120::writeRegister(quint8 address, quint8 value)
{

}

quint8 ads1120::readRegister(quint8 address)
{

}

void ads1120::begin()
{

}

bool ads1120::isDataReady(void)
{

}

quint16 ads1120::readADC(void)
{

}

quint8 *ads1120::readADC_Array(void)
{

}

quint16 ads1120::readADC_Single(void)
{

}

double ads1120::readADC_SingleTemp(void)
{

}

quint8 *ads1120::readADC_SingleArray(void)
{

}

quint16 ads1120::convertToValue(quint8 * data)
{
    return (data[0])<<8 | (data[1]); //Moving MSB and LSB to 16 bit
}

double ads1120::convertToTemp(quint8 * data)
{
    // 14-bit result that is left-justified within the conversion result
     uint16_t conversion = ((data[0])<<8 | (data[1])) >> 2;

     // Negative numbers are represented in binary twos complement format
     if(conversion >= 8192)
     {
       conversion = (~(conversion-1)) ^ 0xC000;
       return conversion * -0.03125;
     }

     return conversion * 0.03125;
}

void ads1120::sendCommand(quint8 command)
{

}

quint8 *ads1120::reset(void)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    cmd_buffer[0] = CMD_RESET;

    return cmd_buffer;
//  sendCommand(CMD_RESET);
}

quint8 *ads1120::startSync(void)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    cmd_buffer[0] = CMD_START_SYNC;

    return cmd_buffer;
//  sendCommand(CMD_START_SYNC);
}

quint8 *ads1120::powerDown(void)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    cmd_buffer[0] = CMD_PWRDWN;

    return cmd_buffer;
//  sendCommand(CMD_PWRDWN);
}

void ads1120::rdata(void)
{
    sendCommand(CMD_RDATA);
}

void ads1120::writeRegisterMasked(quint8 value, quint8 mask, quint8 address)
{
    // Write the value to a register using the mask to leave the rest of the
    // register untouched. This does not shift the value, so it shoudl be provided
    // shifted to the appropriate positions.

    // Read what's in the register now
    uint8_t register_contents = readRegister(address);

    // Flip the mask so that it's zeros where we'll put data and zero out that
    // part of the register's contents
    register_contents = register_contents & ~mask;

    // OR in the value to be written
    register_contents = register_contents | value;

    // Write it back out
    writeRegister(address, register_contents);
}

quint8 * ads1120::setMultiplexer(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Set multiplexer
    | Value | AINp | AINn |
    | ----- | ---- | ---- |
    | 0x00  | AIN0 | AIN1 |
    | 0X01  | AIN0 | AIN2 |
    | 0X02  | AIN0 | AIN3 |
    | 0X03  | AIN1 | AIN2 |
    | 0X04  | AIN1 | AIN3 |
    | 0X05  | AIN2 | AIN3 |
    | 0X06  | AIN1 | AIN0 |
    | 0X07  | AIN3 | AIN2 |
    | 0X08  | AIN0 | AVSS |
    | 0X09  | AIN1 | AVSS |
    | 0X0A  | AIN2 | AVSS |
    | 0X0B  | AIN3 | AVSS |
    | 0X0C  |  REF/4 MON  |
    | 0X0D  | APWR/4 MON  |
    | 0X0E  |   SHORTED   |
    */

    // Make sure the value is in the valid range. Otherwise set to 0x00
    if(value > 0x0E)
    {
      value = 0x00;
    }

    value = value << 4; // Shift to match with mask

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG0_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;

//  writeRegisterMasked(value, REG_MASK_MUX, CONFIG_REG0_ADDRESS);
}

quint8 *ads1120::setGain(quint8 gain)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Sets ADC gain. Possible values are 1, 2, 4, 8, 16, 32, 64, 128. */
     uint8_t value = 0x00;
     switch(gain)
     {
       case 1:
         value = 0x00;
         break;
       case 2:
         value = 0x01;
         break;
       case 4:
         value = 0x02;
         break;
       case 8:
         value = 0x03;
         break;
       case 16:
         value = 0x04;
         break;
       case 32:
         value = 0x05;
         break;
       case 64:
         value = 0x06;
         break;
       case 128:
         value = 0x07;
         break;
       default:
         value = 0x00;
         break;
     }

     value = value << 1; // Shift to match with mask

     cmd_buffer[0] = CMD_WREG | (CONFIG_REG0_ADDRESS << 2);
     cmd_buffer[1] = value;

     return cmd_buffer;

//   writeRegisterMasked(value, REG_MASK_GAIN, CONFIG_REG0_ADDRESS);
}

quint8 *ads1120::setPGAbypass(bool value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Bypasses the PGA if true.
       PGA can only be disabled for gains 1, 2, 4.
    */

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG0_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;
//  writeRegisterMasked(value, REG_MASK_PGA_BYPASS, CONFIG_REG0_ADDRESS);
}

quint8 *ads1120::setDataRate(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Sets the data rate for the ADC. See table 18 in datasheet for datarates
           in various operating modes. */
    // Make sure the value is in the valid range. Otherwise set to 0x00

    if (value > 0x07)
    {
      value = 0x00;
    }

    value = value << 5; // Shift to match with mask

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG1_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;

//  writeRegisterMasked(value, REG_MASK_DATARATE, CONFIG_REG1_ADDRESS);
}

quint8 *ads1120::setOpMode(quint8 value)
{
     memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Sets the ADC operating mode:
       0 - Normal mode
       1 - Duty-cycle mode
       2 - Turbo mode
    */

    // Make sure the value is in the valid range. Otherwise set to 0x00
    if(value > 0x02)
    {
      value = 0x00;
    }

    value = value << 3; // Shift to match with mask

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG1_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;
//  writeRegisterMasked(value, REG_MASK_OP_MODE, CONFIG_REG1_ADDRESS);
}

quint8 *ads1120::setConversionMode(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Sets the ADC conversion mode.
       0 - Single shot mode
       1 - continuous conversion mode
    */

    // Make sure the value is in the valid range. Otherwise set to 0x00
    if(value > 0x01)
    {
      value = 0x00;
    }

    value = value << 2; // Shift to match with mask

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG1_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;
//  writeRegisterMasked(value, REG_MASK_CONV_MODE, CONFIG_REG1_ADDRESS);
}

quint8 *ads1120::setTemperatureMode(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));
    /* Controls the state of the internal temperature sensor.
        0 - Disables temperature sensor
        1 - Enables temperature sensor
    */

     // Make sure the value is in the valid range. Otherwise set to 0x00
     if (value > 0x01)
     {
       value = 0x00;
     }

     value = value << 1; // Shift to match with mask

     cmd_buffer[0] = CMD_WREG | (CONFIG_REG1_ADDRESS << 2);
     cmd_buffer[1] = value;

     return cmd_buffer;

//   writeRegisterMasked(value, REG_MASK_TEMP_MODE, CONFIG_REG1_ADDRESS);
}

quint8 *ads1120::setBurnoutCurrentSources(bool value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));
    /* Turns the 10uA burn-out current sources on or off. */

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG1_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;
//  writeRegisterMasked(value, REG_MASK_BURNOUT_SOURCES, CONFIG_REG1_ADDRESS);
}

quint8 * ads1120::setVoltageRef(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));
    /* Sets the voltage reference used by the ADC.
       0 - Internal 2.048 V
       1 - External on REFP0 and REFN0 inputs
       2 - External on AIN0/REFP1 and AIN3/REFN1 inputs
       3 - Use analog supply as reference
    */

    // Make sure the value is in the valid range. Otherwise set to 0x00
    if(value > 0x03)
    {
      value = 0x00;
    }

    value = value << 6; // Shift to match with mask

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG2_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;

//    writeRegisterMasked(value, REG_MASK_VOLTAGE_REF, CONFIG_REG2_ADDRESS);
}

quint8 * ads1120::setFIR(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Controls the FIR filter on the ADC.
       0 - No 50 or 60 Hz rejection
       1 - Both 50 and 60 Hz rejection
       2 - 50 Hz rejection
       3 - 60 Hz rejection
    */

    // Make sure the value is in the valid range. Otherwise set to 0x00
    if (value > 0x03)
    {
      value = 0x00;
    }

    value = value << 4; // Shift to match with mask

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG2_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;

//  writeRegisterMasked(value, REG_MASK_FIR_CONF, CONFIG_REG2_ADDRESS);
}

quint8 * ads1120::setPowerSwitch(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Configures behavior of low-side switch between AIN3/REFN1 and AVSS.
        0 - Always open
        1 - Automatically closes when START/SYNC command is sent and opens when
            POWERDOWN command is issues.
     */
     // Make sure the value is in the valid range. Otherwise set to 0x00
     if (value > 0x01)
     {
       value = 0x00;
     }
     value = value << 3; // Shift to match with mask

     cmd_buffer[0] = CMD_WREG | (CONFIG_REG2_ADDRESS << 2);
     cmd_buffer[1] = value;

     return cmd_buffer;

//   writeRegisterMasked(value, REG_MASK_PWR_SWITCH, CONFIG_REG2_ADDRESS);
}

quint8 * ads1120::setIDACcurrent(quint8 value)
{    
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));

    /* Set current for both IDAC1 and IDAC2 excitation sources.
       0 - Off
       1 - 10 uA
       2 - 50 uA
       3 - 100 uA
       4 - 250 uA
       5 - 500 uA
       6 - 1000 uA
       7 - 1500 uA
    */

    // Make sure the value is in the valid range. Otherwise set to 0x00
    if (value > 0x07)
    {
      value = 0x00;
    }

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG2_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;

//  writeRegisterMasked(value, REG_MASK_IDAC_CURRENT, CONFIG_REG2_ADDRESS);
}

quint8 * ads1120::setIDAC1routing(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));
    /* Selects where IDAC1 is routed to.
        0 - Disabled
        1 - AIN0/REFP1
        2 - AIN1
        3 - AIN2
        4 - AIN3/REFN1
        5 - REFP0
        6 - REFN0
     */

     // Make sure the value is in the valid range. Otherwise set to 0x00
     if (value > 0x06)
     {
       value = 0x00;
     }

     value = value << 5; // Shift to match with mask

     cmd_buffer[0] = CMD_WREG | (CONFIG_REG3_ADDRESS << 2);
     cmd_buffer[1] = value;

     return cmd_buffer;

//   writeRegisterMasked(value, REG_MASK_IDAC1_ROUTING, CONFIG_REG3_ADDRESS);
}

quint8 * ads1120::setIDAC2routing(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));
    /* Selects where IDAC2 is routed to.
       0 - Disabled
       1 - AIN0/REFP1
       2 - AIN1
       3 - AIN2
       4 - AIN3/REFN1
       5 - REFP0
       6 - REFN0
    */

    // Make sure the value is in the valid range. Otherwise set to 0x00
    if (value > 0x06)
    {
        value = 0x00;
    }

    value = value << 2; // Shift to match with mask

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG3_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;

//  writeRegisterMasked(value, REG_MASK_IDAC2_ROUTING, CONFIG_REG3_ADDRESS);
}

quint8 * ads1120::setDRDYmode(quint8 value)
{
    memset((void *)cmd_buffer, 0x0, sizeof (cmd_buffer));
    if (value > 0x01)
    {
        value = 0x00;
    }

    value = value << 1; // Shift to match with mask

    cmd_buffer[0] = CMD_WREG | (CONFIG_REG3_ADDRESS << 2);
    cmd_buffer[1] = value;

    return cmd_buffer;

//  writeRegisterMasked(value, REG_MASK_DRDY_MODE, CONFIG_REG3_ADDRESS);
}
