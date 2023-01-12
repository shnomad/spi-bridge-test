#ifndef COMMON_H
#define COMMON_H
#include <QDebug>
#include <QDateTime>
#include <QObject>
#include <QThread>

void delay_mSec(int);

#define Log() qDebug() <<"["<<QDateTime::currentDateTime().toString("MM.dd hh:mm:ss") << __PRETTY_FUNCTION__ << __LINE__ << "]"
//#define Log() qDebug()<<"["<< __PRETTY_FUNCTION__ << __LINE__ << "]"
//#define Log() qDebug(logDebug()) <<"["<< __PRETTY_FUNCTION__ << __LINE__ << "]"

using namespace std;

#endif // COMMON_H
