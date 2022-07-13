#include "ft4222.h"
#include <QtDebug>

ft4222::ft4222(QObject *parent) : QObject(parent)
{
//     ListFtUsbDEvices();

//    if(g_FT4222DevList.empty())
//    {
//        qDebug()<<"No FT4222 device is found!\n";
//        exit(0);
//    }

//    OpenEx((PVOID)g_FT4222DevList[0].LocId, FT_OPEN_BY_LOCATION, &g_FT4222DevList[0].ftHandle);
     if(!Open(0))
         exit(0);

     SPI_Master_Init(SS_0);
}

void ft4222::ListFtUsbDEvices()
{
    ftStatus = FT_CreateDeviceInfoList(&numOfDevices);

    if(numOfDevices > 0)
    {
        devInfo = new FT_DEVICE_LIST_INFO_NODE[numOfDevices];
    }
    else
    {
        qDebug()<<"No FT4222 device is found!\n";
        return;
    }

    for(DWORD iDev=0; iDev<numOfDevices; ++iDev)
    {
        FT_DEVICE_LIST_INFO_NODE devInfo;
        memset(&devInfo, 0, sizeof(devInfo));
        ftStatus = FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type, &devInfo.ID,\
                                        &devInfo.LocId, devInfo.SerialNumber, devInfo.Description, &devInfo.ftHandle);

        if (FT_OK == ftStatus)
        {
            qDebug()<<"Dev   :" <<iDev;
            qDebug()<<"Type  :" << QString::number(devInfo.Type, 16);
            qDebug()<<"ID    :" << QString::number(devInfo.ID);
            qDebug()<<"LocId :" << QString::number(devInfo.LocId,16);
            qDebug()<<"SerialNumber :" << QByteArray(devInfo.SerialNumber, sizeof(devInfo.SerialNumber)).toHex(); //.constData();
            qDebug()<<"Description :"<<devInfo.Description;
            qDebug()<<"ftHandle :" << devInfo.ftHandle;
            qDebug()<<"\n";

            QString desc = devInfo.Description;

            if(desc == "FT4222" || desc == "FT4222 A" || desc == "FT4222 B" || desc == "FT4222 C" || desc == "FT4222 D")
            {
                g_FT4222DevList.push_back(devInfo);
            }
        }
    }

}

bool ft4222::Open(quint8 devId)
{
    ftStatus = FT_Open(devId, &ftHandle);

    if (FT_OK != ftStatus)
    {
        return false;
    }

    return true;
}

bool ft4222::OpenEx(PVOID pvArg1, DWORD dwFlags)
{
    ftStatus = FT_OpenEx(pvArg1, dwFlags, &ftHandle);

    if (FT_OK != ftStatus)
    {
        return false;
    }

    return true;
}

bool ft4222::Set_Clock(FT4222_ClockRate clk)
{
    ft4222Status =FT4222_SetClock(ftHandle, clk);

    if(ft4222Status != FT4222_OK)
    {
        FT_Close(ftHandle);
        return false;
    }

    return true;
}

bool ft4222::Get_Clock(FT4222_ClockRate *clk)
{
    ft4222Status =FT4222_GetClock(ftHandle, clk);

    if(ft4222Status != FT4222_OK)
    {
        FT_Close(ftHandle);
        return false;
    }

    return true;
}

bool ft4222::SPI_Master_Init(CS_NUM cs_num)
{
    //ft4222Status = FT4222_SPIMaster_Init(ftHandle, SPI_IO_SINGLE, CLK_DIV_512, CLK_IDLE_HIGH , CLK_TRAILING, cs_num);    //high, high, mode 0
    ft4222Status = FT4222_SPIMaster_Init(ftHandle, SPI_IO_SINGLE, CLK_DIV_512, CLK_IDLE_LOW , CLK_TRAILING, cs_num);   //low,  high, mode 1
    //ft4222Status = FT4222_SPIMaster_Init(ftHandle, SPI_IO_SINGLE, CLK_DIV_512, CLK_IDLE_HIGH , CLK_LEADING, cs_num);   //high, low,  mode 2
    //ft4222Status = FT4222_SPIMaster_Init(ftHandle, SPI_IO_SINGLE, CLK_DIV_512, CLK_IDLE_LOW , CLK_LEADING, cs_num);    //low,  low,  mode 3

    if(FT4222_OK != ft4222Status)
    {
        FT_Close(ftHandle);
        return false;
    }

    return true;
}

bool ft4222::SPI_Single_Write(quint8 *cmd, quint16 cmd_size)
{
    quint16 sizeTransferred;

    ft4222Status = FT4222_SPIMaster_SingleWrite(ftHandle, cmd, cmd_size, &sizeTransferred, true);

    if (FT4222_OK != ft4222Status)
    {
        FT_Close(ftHandle);
        return false;
    }

    return true;
}

bool ft4222::SPI_Single_Read(quint8 *read_buf, quint16 sizeofread)
{
    quint16 sizeTransferred;

    ft4222Status = FT4222_SPIMaster_SingleRead(ftHandle, read_buf, sizeofread, &sizeTransferred, true);

    if (FT4222_OK != ft4222Status)
    {
        FT_Close(ftHandle);
        return false;
    }

    return true;
}

ft4222::~ft4222()
{

}
