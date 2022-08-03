#ifndef COMMON_H
#define COMMON_H
#include <QDebug>
#include <QDateTime>

#ifdef __cplusplus
extern "C" {
#endif

void delay_mSec(int);

#define Log() qDebug() <<"["<<QDateTime::currentDateTime().toString("MM.dd hh:mm:ss") << __PRETTY_FUNCTION__ << __LINE__ << "]"
//#define Log() qDebug()<<"["<< __PRETTY_FUNCTION__ << __LINE__ << "]"
//#define Log() qDebug(logDebug()) <<"["<< __PRETTY_FUNCTION__ << __LINE__ << "]"

#ifdef __cplusplus
}
#endif
#endif // COMMON_H
