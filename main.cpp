#include <QCoreApplication>
//#include "controller.h"
#include "codingchannel.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//  Controller m_controller;

    CodingChannel m_cgms_coding;

    return a.exec();
}
