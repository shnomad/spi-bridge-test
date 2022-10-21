#include "ads130e08.h"

ads130e08::ads130e08(QObject *parent) : QObject(parent)
{

}

quint8 *ads130e08::wakeup()
{
    cmd_buffer[0] = 0x02;
    return cmd_buffer;
}

quint8 *ads130e08::standby()
{
    cmd_buffer[0] = 0x04;
    return cmd_buffer;
}

quint8 *ads130e08::reset()
{
    cmd_buffer[0] = 0x06;
    return cmd_buffer;
}

quint8 *ads130e08::start()
{
    cmd_buffer[0] = 0x08;
    return cmd_buffer;
}

quint8 *ads130e08::stop()
{
    cmd_buffer[0] = 0x0A;
    return cmd_buffer;
}

quint8 *ads130e08::rdatac()
{
    cmd_buffer[0] = 0x10;
    return cmd_buffer;
}

quint8 *ads130e08::sdatac()
{
    cmd_buffer[0] = 0x11;
    return cmd_buffer;
}

quint8 *ads130e08::rdata()
{
    cmd_buffer[0] = 0x12;
    return cmd_buffer;
}
