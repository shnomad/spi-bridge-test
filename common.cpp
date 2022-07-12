#include "common.h"
#include <QObject>
#include <QTimer>
#include <QEventLoop>

void delay_mSec(int msec)
{
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}
