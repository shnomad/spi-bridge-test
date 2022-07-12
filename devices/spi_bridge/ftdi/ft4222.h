#ifndef FT4222_H
#define FT4222_H

#include <QObject>
#include "ftd2xx.h"
#include "libft4222.h"

class ft4222 : public QObject
{
    Q_OBJECT
public:
    explicit ft4222(QObject *parent = nullptr);

    ~ft4222();

    enum CS_NUM
    {
        SS_0 = 0x1,
        SS_1 = 0x2,
        SS_2 = 0x4,
        SS_3 = 0x8
    };

    void ListFtUsbDEvices();
    bool Open(quint8);
    bool OpenEx(PVOID , DWORD);
    bool Set_Clock(FT4222_ClockRate);
    bool Get_Clock(FT4222_ClockRate*);
    bool SPI_Master_Init(CS_NUM);
    bool SPI_Single_Write(quint8 *, quint16);
    bool SPI_Single_Read(quint8 *, quint16);

//signals:

//public slots:

//  private:

    FT_STATUS ftStatus;
    FT4222_STATUS ft4222Status;
    FT_HANDLE ftHandle = nullptr;
    FT4222_ClockRate clk;

    FT_DEVICE_LIST_INFO_NODE *devInfo;
    QVector <FT_DEVICE_LIST_INFO_NODE> g_FT4222DevList;
    DWORD numOfDevices = 0;
};


#endif // FT4222_H
