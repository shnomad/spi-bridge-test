#include "jsondatahandle.h"
#include "buildinfo.h"

jsonDataHandle::jsonDataHandle(QObject *parent) : QObject(parent)
{

}
                                                                    //DAC        , ADC
QString jsonDataHandle::encode_resp(Coding_Channel_Ctl ch_ctl_resp, quint16 *arg1, quint16 *arg2)
{
     QJsonObject mainObject, ADC, DAC;
     QString adc_data_number = "data";

     /* Insert single datas first */
      mainObject.insert("id", ch_ctl_resp.m_ch);
      mainObject.insert("status", ch_ctl_resp.m_resp);
      mainObject.insert("BuildDate",build_date);
      mainObject.insert("Date",QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));

     /* Response for RESP_DAC_CHECK_WORK_SUCCESS, RESP_DAC_OUT_COUNTER_SUCCESS */
    if(ch_ctl_resp.m_resp == Coding_Channel_Ctl::RESP_DAC_CHECK_WORK_SUCCESS || ch_ctl_resp.m_resp == Coding_Channel_Ctl::RESP_DAC_CHECK_COUNTER_SUCCESS \
      || ch_ctl_resp.m_resp == Coding_Channel_Ctl::RESP_DAC_OUT_WORK_SUCCESS || ch_ctl_resp.m_resp == Coding_Channel_Ctl::RESP_DAC_OUT_COUNTER_SUCCESS  \
      || ch_ctl_resp.m_resp == Coding_Channel_Ctl::RESP_START_MEASURE_SUCCESS || ch_ctl_resp.m_resp == Coding_Channel_Ctl::RESP_MEASURED_ADC_VALUE)
    {                
//        DAC.insert("Work", arg1[0]);
//        DAC.insert("Counter",arg1[1]);
          DAC.insert("Work", QString::number(arg1[0], 10));
          DAC.insert("Counter",QString::number(arg1[1], 10));
    }
    else
    {
        DAC.insert("Work","");
        DAC.insert("Counter","");
    }

    mainObject.insert("DAC[mV]", DAC);

    /* RESP_MEASURED_ADC_VALUE */
   if(ch_ctl_resp.m_resp == Coding_Channel_Ctl::RESP_MEASURED_ADC_VALUE)
   {
       for(quint8 seq=0; seq<10; seq++)
       {
           adc_data_number += QString::number(seq, 10);
//         ADC.insert(adc_data_number, arg[seq]);
           ADC.insert(adc_data_number, QString::number(arg2[seq], 16));
           adc_data_number = "data";
       }
//           ADC.insert("MaxData",arg[10]);
//           ADC.insert("MinData",arg[11]);
       ADC.insert("MaxData",QString::number(arg2[10], 16));
       ADC.insert("MinData",QString::number(arg2[11], 16));
   }
   else
   {
     for(quint8 seq=0; seq<10; seq++)
     {
         adc_data_number += QString::number(seq, 10);
         ADC.insert(adc_data_number,"");
         adc_data_number = "data";
     }
         ADC.insert("MaxData","");
         ADC.insert("MinData","");
    }

    mainObject.insert("ADC", ADC);

    QJsonDocument jsonDoc(mainObject);

    /* Write our jsondocument as json with JSON format */
    QString encode_resp = QString::fromUtf8(jsonDoc.toJson());

    return encode_resp;
}

Coding_Channel_Ctl jsonDataHandle::parse(QString message)
{    
     Coding_Channel_Ctl tmp_ch_ctl;

     QJsonDocument jsonDoc =  QJsonDocument::fromJson(message.toUtf8());
     QJsonObject jsonObj = jsonDoc.object();          

     QJsonValue status = jsonObj.value("status");

     Log()<< "status :" << status.toString();

     tmp_ch_ctl.m_cmd = static_cast<Coding_Channel_Ctl::cmd>(status.toInt());

     if(tmp_ch_ctl.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_WORK)
     {
            QJsonObject dac = jsonObj.value("DAC[mV]").toObject();

            QJsonValue dac_work =dac.value("Work");

            tmp_ch_ctl.dac_value_a = dac_work.toInt();
     }
      else if(tmp_ch_ctl.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_COUNTER)
     {
            QJsonObject dac = jsonObj.value("DAC[mV]").toObject();

            QJsonValue dac_counter =dac.value("Counter");

            tmp_ch_ctl.dac_value_b = dac_counter.toInt();
     }

     return tmp_ch_ctl;
}

jsonDataHandle::~jsonDataHandle()
{

}
