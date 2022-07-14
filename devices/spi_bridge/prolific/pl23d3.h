#ifndef PL23D3_H
#define PL23D3_H

#include <QObject>
#include "HidDeviceSdkApi.h"

class pl23d3 : public QObject
{
    Q_OBJECT
public:
    explicit pl23d3(QObject *parent = nullptr);
    ~pl23d3();
    qint32 Init();
    void Get_Library_Version(quint32 *);
    qint32 DeviceCount(quint32 *);
    qint32 DeviceCount(quint32 *, quint16);
    qint32 DeviceCount(quint32 *, quint16, quint16);
    qint32 OpenDevice(quint32, HANDLE *);
    qint32 CloseDevice(HANDLE *);
    qint32 GpioControl(HANDLE, GPIO_PIN, GPIO_DIR, GPIO_VALUE);

signals:

public slots:

   private:
    quint32 *version, *num_device;
};

#endif // PL23D3_H
