#include <QCoreApplication>
//#include "devices/display/sh1106/sh1106.h"
//#include "devices/dac/dac80004/dac80004.h"
#include "devices/dac/dac8562/dac8562.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    dac8562 m_dac;

    m_dac.writeVoltage(200.0f);

    return a.exec();
}
