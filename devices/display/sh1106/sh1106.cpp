
#include "sh1106.h"
#include "../../spi_bridge/ftdi/ft4222.h"

const quint8 sh1106_Initial[] = {0xae, 0x00, 0x10, 0x40, 0x81, 0xcf, 0xa1, 0xc0, 0xa6, 0xa8, 0x3f, 0xd3, 0x00, 0xd5, 0x80, 0xd9, 0xf1, 0xda, 0x12, 0xdb, 0x40, 0x20, 0x02, 0x8d, 0x14, 0xa4, 0xa6, 0xaf};
//const quint8 sh1106_Initial[] = {0xae, 0x02, 0x10, 0x40, 0x81, 0xa0, 0xc0, 0xa6, 0xa8, 0x3f, 0xd3, 0x00, 0xd5, 0x80, 0xd9, 0xf1, 0xda, 0x12, 0xdb, 0x40, 0x20, 0x02, 0xa4, 0xa6};
const quint8 sh1106_set_start_address[] = {0x02, 0x10};
const quint8 sh1106_dispaly_on[] =  {0x8d, 0x14, 0xaf};
const quint8 sh1106_dispaly_off[] = {0x8d, 0x10, 0xae};
const quint8 sh1106_set_page_address[] ={0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7};

sh1106::sh1106(QObject *parent) : GpioControl(parent)
{
    HW_Init();
    Reset();
    m_spi_control = new ft4222;
}

void sh1106::Reset(void)
{
    GpioControl::setValue(3, GpioControl::SET_VAL::SET_HIGH);
    GpioControl::setValue(3, GpioControl::SET_VAL::SET_LOW);
//  delay_mSec(300);
    GpioControl::setValue(3, GpioControl::SET_VAL::SET_HIGH);
}

void sh1106::HW_Init(void)
{
    // DC PIN : RPi's GPIO2
    GpioControl::exportGPIO(2);
    GpioControl::setDirection(2, GpioControl::SET_DIR::SET_OUT);
    GpioControl::setValue(2, GpioControl::SET_VAL::SET_HIGH);

    // RESET PIN : RPi's GPIO3
    GpioControl::exportGPIO(3);
    GpioControl::setDirection(3, GpioControl::SET_DIR::SET_OUT);
    GpioControl::setValue(3, GpioControl::SET_VAL::SET_HIGH);
}

bool sh1106::Write_Byte(sh1106::CMD_DATA_SEL cmd_data, quint8 *spi_cmd, quint16 cmd_size)
{

    if(cmd_data == CMD)
        GpioControl::setValue(2, GpioControl::SET_VAL::SET_LOW);
    else
        GpioControl::setValue(2, GpioControl::SET_VAL::SET_HIGH);

    if(!m_spi_control->SPI_Single_Write(spi_cmd, cmd_size))
    {
        return false;
    }
    return true;
}

void sh1106::Display_Init(void)
{

#if 1
     Write_Byte(CMD, (quint8 *)sh1106_Initial,sizeof(sh1106_Initial));
#else
    for(quint8 i=0; i<sizeof(sh1106_Initial); i++)
    {
         Write_Byte(CMD, (quint8 *)(sh1106_Initial + i),0x1);

         delay_mSec(30);
    }

#endif

//   Clear_Screen(0x0);
}

void sh1106::Clear_Screen(quint8 color)
{
    quint8 i, j;

    for (i=0; i<8; i++)
    {
        /* set page address */
        Write_Byte(CMD,(quint8*)(sh1106_set_page_address + i), 0x1);

        /* set low column, high column address */
        Write_Byte(CMD,(quint8*)sh1106_set_start_address, sizeof(sh1106_set_start_address));

        for(j=0; j<128; j++)
        {
            /* write data */
//          OLED_WriteData(0x00);
            Write_Byte(DATA,(quint8*)&color, 0x1);
        }

    }
}

sh1106::~sh1106()
{

}
