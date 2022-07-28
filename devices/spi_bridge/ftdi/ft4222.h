#ifndef FT4222_H
#define FT4222_H

#include <QObject>
#include "ftd2xx.h"
#include "libft4222.h"

class ft4222 : public QObject
{
    Q_OBJECT
public:
    enum CS_NUM
    {
        SS_0 = 0x1,
        SS_1 = 0x2,
        SS_2 = 0x4,
        SS_3 = 0x8
    };

    enum DEV_NUM
    {
        dev_0 = 0x0,
        dev_1,
        dev_2,
        dev_3
    };

    explicit ft4222(QObject *parent = nullptr);
    ~ft4222();

    quint8 CreatDEviceInfoList();
    void GetDeviceInfoDetail(quint8, FT_DEVICE_LIST_INFO_NODE *);
    FT_STATUS Open(quint8, FT_HANDLE*);
    FT_STATUS OpenEx(PVOID , DWORD, FT_HANDLE*);
    FT4222_STATUS Set_Clock(FT_HANDLE, FT4222_ClockRate);
    FT4222_STATUS Get_Clock(FT_HANDLE, FT4222_ClockRate*);
    FT4222_STATUS SPI_Master_Init(FT_HANDLE, quint8);
    FT4222_STATUS SPI_Single_Write(FT_HANDLE, quint8 *, quint16);
    FT4222_STATUS SPI_Single_Read(FT_HANDLE, quint8 *, quint16);

//signals:

//public slots:

//  private:
    FT_STATUS ftStatus;
    FT4222_STATUS ft4222Status;
//  FT_HANDLE ftHandle;
    FT4222_ClockRate clk;

    FT_DEVICE_LIST_INFO_NODE *devInfo;
    QVector <FT_DEVICE_LIST_INFO_NODE> g_FT4222DevList;
    DWORD numOfDevices = 0;
};


#endif // FT4222_H
