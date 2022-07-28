#include "ft4222.h"
#include <QtDebug>

ft4222::ft4222(QObject *parent) : QObject(parent)
{
//  if(!Open(DevId, fthandle))
//  exit(0);
//  SPI_Master_Init(fthandle, (quint8)0x1 << DevId);
}

quint8 ft4222::CreatDEviceInfoList()
{
    quint8 dev_num = 0;

    ftStatus = FT_CreateDeviceInfoList(&numOfDevices);

    if(numOfDevices > 0)
        dev_num = static_cast<quint8>(numOfDevices);
    else   
        qDebug()<<"No FT4222 device is found!\n";            

    /*
    for(DWORD iDev=0; iDev<numOfDevices; ++iDev)
    {
        FT_DEVICE_LIST_INFO_NODE devInfo[iDev];

        memset(&devInfo[iDev], 0, sizeof(devInfo[iDev]));

        ftStatus = FT_GetDeviceInfoDetail(iDev, &devInfo[iDev].Flags, &devInfo[iDev].Type, &devInfo[iDev].ID,\
                                        &devInfo[iDev].LocId, devInfo[iDev].SerialNumber, devInfo[iDev].Description, &devInfo[iDev].ftHandle);

        if (FT_OK == ftStatus)
        {
            qDebug()<<"Dev   :" <<iDev;
            qDebug()<<"Type  :" << QString::number(devInfo[iDev].Type, 16);
            qDebug()<<"ID    :" << QString::number(devInfo[iDev].ID);
            qDebug()<<"LocId :" << QString::number(devInfo[iDev].LocId,16);
            qDebug()<<"SerialNumber :" << QByteArray(devInfo[iDev].SerialNumber, sizeof(devInfo[iDev].SerialNumber)).toHex(); //.constData();
            qDebug()<<"Description :"<<devInfo[iDev].Description;
            qDebug()<<"ftHandle :" << devInfo[iDev].ftHandle;
            qDebug()<<"\n";

            QString desc = devInfo[iDev].Description;

            if(desc == "FT4222" || desc == "FT4222 A" || desc == "FT4222 B" || desc == "FT4222 C" || desc == "FT4222 D")
            {
                g_FT4222DevList.push_back(devInfo[iDev]);
            }
        }
    }
    */
    return dev_num;
}

void ft4222::GetDeviceInfoDetail(quint8 numOfDevices, FT_DEVICE_LIST_INFO_NODE *devInfo)
{

    for(DWORD iDev=0; iDev<numOfDevices; ++iDev)
    {
        //FT_DEVICE_LIST_INFO_NODE devInfo[iDev];

        memset(&devInfo[iDev], 0, sizeof(devInfo[iDev]));

        ftStatus = FT_GetDeviceInfoDetail(iDev, &devInfo[iDev].Flags, &devInfo[iDev].Type, &devInfo[iDev].ID,\
                                        &devInfo[iDev].LocId, devInfo[iDev].SerialNumber, devInfo[iDev].Description, &devInfo[iDev].ftHandle);

        if (FT_OK == ftStatus)
        {
            qDebug()<<"Dev   :" <<iDev;
            qDebug()<<"Type  :" << QString::number(devInfo[iDev].Type, 16);
            qDebug()<<"ID    :" << QString::number(devInfo[iDev].ID);
            qDebug()<<"LocId :" << QString::number(devInfo[iDev].LocId,16);
            qDebug()<<"SerialNumber :" << QByteArray(devInfo[iDev].SerialNumber, sizeof(devInfo[iDev].SerialNumber)).toHex(); //.constData();
            qDebug()<<"Description :"<<devInfo[iDev].Description;
            qDebug()<<"ftHandle :" << devInfo[iDev].ftHandle;
            qDebug()<<"\n";
        }
    }

}

FT_STATUS ft4222::Open(quint8 devId, FT_HANDLE *fthandle)
{
    return  FT_Open(devId, fthandle);
}

FT_STATUS ft4222::OpenEx(PVOID pvArg1, DWORD dwFlags, FT_HANDLE *fthandle)
{
    return  ftStatus = FT_OpenEx(pvArg1, dwFlags, fthandle);
}

FT4222_STATUS ft4222::Set_Clock(FT_HANDLE fthandle, FT4222_ClockRate clk)
{
    ft4222Status =FT4222_SetClock(fthandle, clk);

    if(ft4222Status != FT4222_OK)
    {
        FT_Close(fthandle);
        return ft4222Status;
    }

    return ft4222Status;
}

FT4222_STATUS ft4222::Get_Clock(FT_HANDLE ftHandle, FT4222_ClockRate *clk)
{
    ft4222Status =FT4222_GetClock(ftHandle, clk);

    if(ft4222Status != FT4222_OK)
    {
        FT_Close(ftHandle);
        return ft4222Status;
    }

    return ft4222Status;
}

FT4222_STATUS ft4222::SPI_Master_Init(FT_HANDLE ftHandle, quint8 cs_num)
{
    //ft4222Status = FT4222_SPIMaster_Init(ftHandle, SPI_IO_SINGLE, CLK_DIV_512, CLK_IDLE_HIGH , CLK_TRAILING, cs_num);  //high, high, mode 0
      ft4222Status = FT4222_SPIMaster_Init(ftHandle, SPI_IO_SINGLE, CLK_DIV_32, CLK_IDLE_LOW , CLK_TRAILING, cs_num);     //low,  high, mode 1   //DAC8562
    //ft4222Status = FT4222_SPIMaster_Init(ftHandle, SPI_IO_SINGLE, CLK_DIV_512, CLK_IDLE_HIGH , CLK_LEADING, cs_num);   //high, low,  mode 2
    //ft4222Status = FT4222_SPIMaster_Init(ftHandle, SPI_IO_SINGLE, CLK_DIV_512, CLK_IDLE_LOW , CLK_LEADING, cs_num);    //low,  low,  mode 3

    if(FT4222_OK != ft4222Status)
    {
        FT_Close(ftHandle);
        return ft4222Status;
    }

    return ft4222Status;
}

FT4222_STATUS ft4222::SPI_Single_Write(FT_HANDLE ftHandle, quint8 *cmd, quint16 cmd_size)
{
    quint16 sizeTransferred;

    ft4222Status = FT4222_SPIMaster_SingleWrite(ftHandle, cmd, cmd_size, &sizeTransferred, true);

    if (FT4222_OK != ft4222Status)
    {
        FT_Close(ftHandle);
        return ft4222Status;
    }

    return ft4222Status;
}

FT4222_STATUS ft4222::SPI_Single_Read(FT_HANDLE ftHandle, quint8 *read_buf, quint16 sizeofread)
{
    quint16 sizeTransferred;

    ft4222Status = FT4222_SPIMaster_SingleRead(ftHandle, read_buf, sizeofread, &sizeTransferred, true);

    if (FT4222_OK != ft4222Status)
    {
        FT_Close(ftHandle);
        return ft4222Status;
    }

    return ft4222Status;
}

ft4222::~ft4222()
{

}
