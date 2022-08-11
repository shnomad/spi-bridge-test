#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "jsondatahandle.h"
#include "common.h"

jsonDataHandle::jsonDataHandle(QObject *parent) : QObject(parent)
{

}

QString jsonDataHandle::create(quint16 channel, quint16 adc_value, float dac_value_a, float dac_value_b)
{
     QJsonObject mainObject;

    /* insert single datas first */
     mainObject.insert("date",QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));
     mainObject.insert("channel", channel);
     mainObject.insert("ADC", QString("%1").arg(adc_value, 0, 16));
     mainObject.insert("DAC_A", QString().setNum(dac_value_a,'g'));
     mainObject.insert("DAC_B", QString().setNum(dac_value_b,'g'));

    /* lastly we created a JSON document and set mainObject as object of document */
     QJsonDocument jsonDoc(mainObject);

    /* Write our jsondocument as json with JSON format */
     QString encode_data = QString::fromUtf8(jsonDoc.toJson());

     Log()<<encode_data;

     return encode_data;
}

QJsonObject jsonDataHandle::parse(QString message)
{
     QJsonDocument jsonDoc =  QJsonDocument::fromJson(message.toUtf8());
     QJsonObject jsonObj = jsonDoc.object();

     return jsonObj;
}

jsonDataHandle::~jsonDataHandle()
{

}
