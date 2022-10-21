#ifndef ADS130E08_H
#define ADS130E08_H

#include <QObject>

class ads130e08 : public QObject
{
    Q_OBJECT
public:
    explicit ads130e08(QObject *parent = nullptr);

    /*System Command*/
    quint8 *wakeup();
    quint8 *standby();
    quint8 *reset();
    quint8 *start();
    quint8 *stop();

    /*Data Read Command*/
    quint8 *rdatac();
    quint8 *sdatac();
    quint8 *rdata();

    /*Register Read/Write Command*/

signals:

public slots:

private:
    quint8 cmd_buffer[1];
};

#endif // ADS130E08_H
