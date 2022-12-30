#include "jsondatahandle.h"
#include "buildinfo.h"

const QString jsonDataHandle::filename = "/home/user/config.txt";

jsonDataHandle::jsonDataHandle(QObject *parent) : QObject(parent)
{                    

}
                                                                            //ADC
QString jsonDataHandle::encode_resp(Coding_Channel_Ctl ch_ctl_resp, quint16 *arg2)
{
     QJsonObject mainObject, ADC, DAC;
     QString adc_data_number = "data";

     /* Insert single datas first */
      mainObject.insert("channel", ch_ctl_resp.m_ch);
      mainObject.insert("response", ch_ctl_resp.m_resp);
      mainObject.insert("BuildDate",build_date);
      mainObject.insert("Date",QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));

    if((ch_ctl_resp.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_WORK_VOLT) || (ch_ctl_resp.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_COUNTER_VOLT) || \
        (ch_ctl_resp.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_WORK_BIT) || (ch_ctl_resp.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_COUNTER_BIT) )
    {
        DAC.insert("Work[mV]","");
        DAC.insert("Counter[mV]","");
        DAC.insert("Work[bit]","");
        DAC.insert("Counter[bit]","");
    }

    mainObject.insert("DAC", DAC);

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
//         ADC.insert("MaxData",arg[10]);
//         ADC.insert("MinData",arg[11]);
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

     QJsonValue command = jsonObj.value("command");
     QJsonValue channel = jsonObj.value("channel");

     Log()<< "command :" << command.toInt();
     Log()<< "channel :" << channel.toString();

     tmp_ch_ctl.m_cmd = static_cast<Coding_Channel_Ctl::cmd>(command.toInt());   

     if(tmp_ch_ctl.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_WORK_VOLT)
     {
         QJsonObject dac = jsonObj.value("DAC").toObject();

         QJsonValue dac_work =dac.value("Work[mV]");

         tmp_ch_ctl.dac_value_a = dac_work.toInt();

         /*Update config.txt file*/
         Update(channel.toString(),"Work[mV]",tmp_ch_ctl.dac_value_a, jsonDataHandle::dac);

         goto SendToAfe;
     }

     if(tmp_ch_ctl.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_COUNTER_VOLT)
    {
        QJsonObject dac = jsonObj.value("DAC").toObject();

        QJsonValue dac_counter =dac.value("Counter[mV]");

        tmp_ch_ctl.dac_value_b = dac_counter.toInt();

        /*Update config.txt file*/
        Update(channel.toString(), "Counter[mV]",tmp_ch_ctl.dac_value_b, jsonDataHandle::dac);

        goto SendToAfe;
     }

    if(tmp_ch_ctl.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_WORK_BIT)
     {
        QJsonObject dac = jsonObj.value("DAC").toObject();

        QJsonValue dac_work =dac.value("Work[bit]");

        tmp_ch_ctl.dac_value_a_r = dac_work.toInt();

        /*Update config.txt file*/
        Update(channel.toString(), "Work[bit]",tmp_ch_ctl.dac_value_a_r, jsonDataHandle::dac);

        goto SendToAfe;
    }

    if(tmp_ch_ctl.m_cmd == Coding_Channel_Ctl::CMD_DAC_OUT_COUNTER_BIT)
    {
        QJsonObject dac = jsonObj.value("DAC").toObject();

        QJsonValue dac_counter =dac.value("Counter[bit]");

        tmp_ch_ctl.dac_value_b_r = dac_counter.toInt();

        /*Update config.txt file*/
        Update(channel.toString(), "Counter[bit]",tmp_ch_ctl.dac_value_b_r, jsonDataHandle::dac);
     }

     SendToAfe:

     return tmp_ch_ctl;
}

bool jsonDataHandle::Create(quint8 board_number)
{
    QJsonObject mainObject, CHANNEL, CAL, DAC;
    QJsonDocument jsonDoc;
    QFile file;

    file.setFileName(filename);

    if(!file.open(QIODevice::ReadWrite))
        return false;

    DAC.insert("Work[mV]","");
    DAC.insert("Counter[mV]","");
    DAC.insert("Work[bit]","");
    DAC.insert("Counter[bit]","");

    CAL.insert("slope","");
    CAL.insert("intercept","");

    CHANNEL.insert("DAC",DAC);
    CHANNEL.insert("Calibrarion",CAL);

    for(quint8 count=0; count<15; count++)
    {
        mainObject.insert(total_channel[board_number][count],CHANNEL);
    }

    jsonDoc.setObject(mainObject);

    file.write(jsonDoc.toJson());

    file.close();

    return true;
}

QJsonDocument jsonDataHandle::Open(const bool is_write)
{    
    QFile file;
    QString load_data;

//  const QString filename = "/home/user/config_" + QString("%1").arg(channels, 3, 10, QChar('0')) + ".txt";

    file.setFileName(filename);

    if(is_write)
    {
        if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
            exit(0);
     }
    else
    {
        if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
            exit(0);
    }

    load_data = file.readAll();

    file.close();

    return QJsonDocument::fromJson(load_data.toUtf8());
}

bool jsonDataHandle::Update(QString channel, QString key_, quint16 val_, update_sel dac_cal)
{
    QFile file;

    QJsonDocument jsonDoc_temp = Open(false);    
    QJsonObject mainObject = jsonDoc_temp.object();
    QJsonObject ch_tmp = mainObject[channel].toObject();

    if(dac_cal == jsonDataHandle::dac)
    {
        QJsonObject dac_tmp = ch_tmp["DAC"].toObject();

        dac_tmp[key_] = val_;

        ch_tmp["DAC"]=dac_tmp;

    }
    else
    {
       QJsonObject cal_tmp = ch_tmp["Calibration"].toObject();

       cal_tmp[key_] = val_;

       ch_tmp["Calibration"]=cal_tmp;
    }

    mainObject[channel]=ch_tmp;

    jsonDoc_temp.setObject(mainObject);

    /*write to config.txt*/
    file.setFileName(filename);

    //if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    if(!file.open(QIODevice::ReadWrite))
        exit(0);

    file.write(jsonDoc_temp.toJson());

    file.close();

    return true;
}

jsonDataHandle::~jsonDataHandle()
{

}
