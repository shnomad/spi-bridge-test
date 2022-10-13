#ifndef DAC8562_H
#define DAC8562_H

#include <QObject>

#define CMD_SETA_UPDATEA          0x18  // A通道命令+16位A路数据
#define CMD_SETB_UPDATEB          0x19  // B通道命令+16位B路数据
#define CMD_UPDATE_ALL_DACS       0x0F  // 更新两路寄存器命令，后16位只需时钟即可

#define CMD_GAIN                  0x02  // 内部放大倍数命令
#define DATA_GAIN_B2_A2           0x0000  // B路2倍，A路1倍
#define DATA_GAIN_B2_A1           0x0001  // B路1倍，A路2倍
#define DATA_GAIN_B1_A2           0x0002  // B路2倍，A路2倍
#define DATA_GAIN_B1_A1           0x0003  // B路1倍，A路1倍

#define CMD_PWR_UP_A_B            0x20  // 命令：上电A、B路
#define DATA_PWR_UP_A_B           0x0003  // 数据：Power up DAC-A and DAC-B  data

#define CMD_PWR_DOWN_A_B          0x20    // power down Hi-
#define DATA_PWR_DOWN_A_B         0x001B  // 数据：Power Down DAC-A and DAC-B  data

#define CMD_RESET_ALL_REG         0x28  // 命令：所有寄存器复位、清空寄存器
#define DATA_RESET_ALL_REG        0x0001  // 数据：所有寄存器复位、清空寄存器

#define CMD_LDAC_DIS              0x30  // LDAC脚功能命令
#define DATA_LDAC_DIS             0x0003  // LDAC脚不起作用

#define CMD_INTERNAL_REF_DIS      0x38  // 命令：Disable internal reference and reset DACs to gain = 1
#define DATA_INTERNAL_REF_DIS     0x0000  // 数据：Disable internal reference and reset DACs to gain = 1
#define CMD_INTERNAL_REF_EN       0x38  // 命令：Enable Internal Reference & reset DACs to gain = 2
#define DATA_INTERNAL_REF_EN      0x0001  // 数据：Enable Internal Reference & reset DACs to gain = 2

//class ft4222;

class dac8562 : public QObject
{
    Q_OBJECT
public:
     explicit dac8562(QObject *parent = nullptr);
//    explicit dac8562(int, QObject *parent = nullptr);
    ~dac8562();    
    void initialize();           
    quint8 *DAC_WR_REG(quint8 cmd_byte, quint16 data_byte);
    quint8 *outPutValue(quint8 cmd_byte, quint16 input);
    quint8 * writeA(float input);
    quint8 * writeB(float input);
    quint8 * writeALL(float input);
    quint16 Voltage_Convert(float voltage);

 // void writeVoltage(float input);
 // void DAC_WR_REG(quint8 cmd_byte, quint16 data_byte, quint8 *cmd_buf);

signals:

public slots:

private:    
    quint8 cmd_buffer[3];
    int spi_fd;
};

#endif // DAC8562_H
