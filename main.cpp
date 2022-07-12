#include <QCoreApplication>
//#include "devices/display/sh1106/sh1106.h"
#include "devices/dac/dac80004/dac80004.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#if 0
    sh1106 m_disply;

    m_disply.Display_Init();

    m_disply.Clear_Screen(0xff);

    m_disply.Clear_Screen(0x0);
#endif

    dac80004 m_dac;

//  m_dac.PowerUpDown(true);

    m_dac.WriteToBuffer(50000, m_dac.CH_A, true);

    return a.exec();
}
