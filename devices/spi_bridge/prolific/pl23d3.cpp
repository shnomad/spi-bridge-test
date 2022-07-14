#include "pl23d3.h"

pl23d3::pl23d3(QObject *parent) : QObject(parent)
{
    Init();
//  Get_Library_Version(version);
    DeviceCount(num_device, 0x067b, 0x2362);
}

qint32 pl23d3::Init()
{
    qint32 result = InitDevice();

    return result;
}

void pl23d3::Get_Library_Version(quint32 *version)
{
    GetSDKVersion(version);
}

qint32 pl23d3::DeviceCount(quint32 *count)
{
      qint32 result = EnumHidDeviceArray(count);

      return result;
}

qint32 pl23d3::DeviceCount(quint32 *count, quint16 vid)
{
    qint32 result = EnumDeviceByVid(count, vid);

    return result;
}

qint32 pl23d3::DeviceCount(quint32 *count, quint16 vid, quint16 pid)
{
    qint32 result = EnumDeviceByVidPid(count, vid, pid);

    return result;
}

qint32 pl23d3::OpenDevice(quint32 index, HANDLE *handle)
{
    qint32 result = OpenDeviceHandle(index, handle);

    return result;
}

qint32 pl23d3::GpioControl(HANDLE handle, GPIO_PIN pin, GPIO_DIR dir, GPIO_VALUE value)
{
    quint8 *read_val = nullptr;

    qint32 result = SetGPIODir(handle, pin, dir);
    if(result < ERROR_SUCCESS)
        return result;

    if(dir == GPIO_OUTPUT_DIR)
    {
        result = SetGPIO(handle, pin, value);
        if(result < ERROR_SUCCESS)
            return result;
    }
    else
    {
        result = GetGPIO(handle, pin, read_val);
        if(result < ERROR_SUCCESS)
            return result;
    }

    return result;
}

qint32 pl23d3::CloseDevice(HANDLE *handle)
{
    qint32 result = CloseDeviceHandle(handle);

    return result;
}

pl23d3::~pl23d3()
{

}
