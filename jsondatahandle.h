#ifndef JSONDATAHANDLE_H
#define JSONDATAHANDLE_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "common.h"

class jsonDataHandle : public QObject
{
    Q_OBJECT
public:
    explicit jsonDataHandle(QObject *parent = nullptr);
    ~jsonDataHandle();

    //void create(quint16, quint16, float, float, QString &);
    QString encode_resp(Coding_Channel_Ctl, quint16 *, quint16 *);
    Coding_Channel_Ctl parse(QString);

signals:

public slots:


private:
Coding_Channel_Ctl ch_ctl_cmd{};

};

#endif // JSONDATAHANDLE_H
