#ifndef BUILDINFO_H
#define BUILDINFO_H

#include <QString>
#include <QLocale>

QString build_date = QString("%1T%2").arg(__DATE__).arg(__TIME__);

#endif // BUILDINFO_H
