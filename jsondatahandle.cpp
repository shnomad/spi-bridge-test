#include "jsondatahandle.h"
#include "buildinfo.h"

const QString jsonDataHandle::filename = "/home/user/config.txt";

jsonDataHandle::jsonDataHandle(QObject *parent) : QObject(parent)
{                    
    cmd_from_host_parsing = new sys_cmd_resp;
}
                                                                       //ADC
QString jsonDataHandle::encode_resp(sys_cmd_resp *resp_to_host, quint16 *arg2)
{
    QJsonObject mainObject, ADC, DAC, CAL;
    QString adc_data_number = "data";

    /* Insert single data first */
    mainObject.insert("channel", resp_to_host->m_ch);
    mainObject.insert("response", resp_to_host->m_resp);
    mainObject.insert("BuildDate",build_date);
    mainObject.insert("Date",QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ap"));

    switch(resp_to_host->m_resp)
    {        

        case sys_cmd_resp::RESP_START_MEASURE_SUCCESS:
        break;

        case sys_cmd_resp::RESP_STOP_MEASURE_SUCCESS:
        break;

        case sys_cmd_resp::RESP_DAC_OUT_WORK_SUCCESS:
        case sys_cmd_resp::RESP_DAC_OUT_COUNTER_SUCCESS:
            {

            }
        break;

        case sys_cmd_resp::RESP_DAC_VOLT_CHECK_SUCCESS:
        case sys_cmd_resp::RESP_DAC_BIT_CHECK_SUCCESS:
            {
                QFile file;

                /*check config file and read saved parameters*/
                if(file.exists(filename))
                {
                    QJsonDocument jsonDoc_from_config = Open(false);
                    QJsonObject mainObject_from_config = jsonDoc_from_config.object();
                    QJsonObject ch_param_from_config = mainObject_from_config[QMetaEnum::fromType<sys_cmd_resp::channel>().valueToKey(resp_to_host->m_ch)].toObject();
                    QJsonObject dac_val_from_config = ch_param_from_config["DAC"].toObject();

                    DAC.insert("Work[mV]",dac_val_from_config["Work[mV]"]);
                    DAC.insert("Counter[mV]",dac_val_from_config["Counter[mV]"]);
                    DAC.insert("Work[bit]",dac_val_from_config["Work[bit]"]);
                    DAC.insert("Counter[bit]",dac_val_from_config["Counter[bit]"]);
                }
                else
                {
                    DAC.insert("Work[mV]","");
                    DAC.insert("Counter[mV]","");
                    DAC.insert("Work[bit]","");
                    DAC.insert("Counter[bit]","");
                }

               mainObject.insert("DAC", DAC);
            }
        break;

        case sys_cmd_resp::RESP_MEASURED_ADC_VALUE:
            {
                for(quint8 seq=0; seq<10; seq++)
                {
                    adc_data_number += QString::number(seq, 10);
                    ADC.insert(adc_data_number, QString::number(arg2[seq], 16));
                    adc_data_number = "data";
                }
                    ADC.insert("MaxData",QString::number(arg2[10], 16));
                    ADC.insert("MinData",QString::number(arg2[11], 16));

                    QFile file;

                    if(file.exists(filename))
                    {
                        QJsonDocument jsonDoc_from_config = Open(false);
                        QJsonObject mainObject_from_config = jsonDoc_from_config.object();
                        QJsonObject ch_param_from_config = mainObject_from_config[QMetaEnum::fromType<sys_cmd_resp::channel>().valueToKey(resp_to_host->m_ch)].toObject();
                        QJsonObject cal_val_from_config = ch_param_from_config["CAL"].toObject();
                        QJsonObject dac_val_from_config = ch_param_from_config["DAC"].toObject();

                        CAL.insert("slope", cal_val_from_config["slope"]);
                        CAL.insert("intercept", cal_val_from_config["intercept"]);

                        DAC.insert("Work[mV]", dac_val_from_config["Work[mV]"]);
                        DAC.insert("Counter[mV]", dac_val_from_config["Counter[mV]"]);
                        DAC.insert("Work[bit]", dac_val_from_config["Work[bit]"]);
                        DAC.insert("Counter[bit]",dac_val_from_config["Counter[bit]"]);
                    }
                    else
                    {
                        CAL.insert("slope", "");
                        CAL.insert("intercept","");

                        DAC.insert("Work[mV]","");
                        DAC.insert("Counter[mV]","");
                        DAC.insert("Work[bit]","");
                        DAC.insert("Counter[bit]","");
                    }

                mainObject.insert("DAC", DAC);
                mainObject.insert("ADC", ADC);
                mainObject.insert("CAL", CAL);

            }
        break;

        case sys_cmd_resp::RESP_BATTERY_CHECK_SUCCESS:
        break;

        case sys_cmd_resp::RESP_FW_CHECK_SUCCESS:
        break;

        case sys_cmd_resp::RESP_AFE_READY_SUCCESS:
        case sys_cmd_resp::RESP_AFE_READY_FAIL:
            {
                 QJsonArray resp_arg;

                 resp_arg.push_back(static_cast<quint16>(resp_to_host->resp_arg[0]));
                 resp_arg.push_back(static_cast<quint16>(resp_to_host->resp_arg[1]));
                 resp_arg.push_back(static_cast<quint16>(resp_to_host->resp_arg[2]));
                 resp_arg.push_back(static_cast<quint16>(resp_to_host->resp_arg[3]));
                 resp_arg.push_back(resp_to_host->resp_arg[4]);
                 resp_arg.push_back(resp_to_host->resp_arg[5]);

                 mainObject.insert("response_detail", resp_arg);
            }
        break;

        case sys_cmd_resp::RESP_START_MEASURE_FAIL:
        case sys_cmd_resp::RESP_STOP_MEASURE_FAIL:
        case sys_cmd_resp::RESP_DAC_OUT_WORK_FAIL:
        case sys_cmd_resp::RESP_DAC_CHECK_WORK_FAIL:
        case sys_cmd_resp::RESP_DAC_OUT_COUNTER_FAIL:
        case sys_cmd_resp::RESP_DAC_CHECK_COUNTER_FAIL:
        case sys_cmd_resp::RESP_FW_CHECK_FAIL:
        case sys_cmd_resp::RESP_CHECK_AFE_CONNECTED:
        case sys_cmd_resp::RESP_BATTERY_CHECK_FAIL:
        break;

        case sys_cmd_resp::RESP_UNKNOWN:
        break;
    }

    QJsonDocument jsonDoc(mainObject);

    /* Write our jsondocument as json with JSON format */
    QString encode_resp = QString::fromUtf8(jsonDoc.toJson());

    return encode_resp;
}
                                                                          //ADC
sys_cmd_resp *jsonDataHandle::parse(QString message)
{

    QJsonDocument jsonDoc =  QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();

    QJsonValue command = jsonObj.value("command");
    QJsonValue channel = jsonObj.value("channel");

//  QString ch_number = channel.toString();

    Log()<<"channel :" <<QMetaEnum::fromType<sys_cmd_resp::channel>().valueToKey(channel.toInt());
    Log()<<"command :" <<QMetaEnum::fromType<sys_cmd_resp::cmd>().valueToKey(command.toInt());

    cmd_from_host_parsing->m_ch = static_cast<sys_cmd_resp::channel>(channel.toInt());
    cmd_from_host_parsing->m_cmd = static_cast<sys_cmd_resp::cmd>(command.toInt());

    switch(cmd_from_host_parsing->m_cmd)
    {
        case sys_cmd_resp::CMD_AFE_READY:
            {

            }
        break;

        case sys_cmd_resp::CMD_START_MEASURE:
        case sys_cmd_resp::CMD_STOP_MEASURE:
        break;

        case sys_cmd_resp::CMD_DAC_VOLT_CHECK:
            {

            }
        break;

        case sys_cmd_resp::CMD_DAC_BIT_CHECK:
            {

            }
        break;

        case sys_cmd_resp::CMD_DAC_OUT_COUNTER_VOLT:
        case sys_cmd_resp::CMD_DAC_OUT_WORK_VOLT:
        case sys_cmd_resp::CMD_DAC_OUT_COUNTER_BIT:
        case sys_cmd_resp::CMD_DAC_OUT_WORK_BIT:
        {
             QJsonObject dac = jsonObj.value("DAC").toObject();

             if(cmd_from_host_parsing->m_cmd == sys_cmd_resp::CMD_DAC_OUT_WORK_VOLT)
             {
                 QJsonValue dac_work =dac.value("Work[mV]");

                 cmd_from_host_parsing->dac_value_a_v = static_cast<float>(dac_work.toInt());

                 /*Update config.txt file*/
                 Update(QMetaEnum::fromType<sys_cmd_resp::channel>().valueToKey(channel.toInt()),"Work[mV]", cmd_from_host_parsing->dac_value_a_v, jsonDataHandle::dac);
             }
             else if(cmd_from_host_parsing->m_cmd == sys_cmd_resp::CMD_DAC_OUT_COUNTER_VOLT)
             {
                 QJsonValue dac_counter =dac.value("Counter[mV]");

                 cmd_from_host_parsing->dac_value_b_v = static_cast<float>(dac_counter.toInt());

                 /*Update config.txt file*/
                 Update(QMetaEnum::fromType<sys_cmd_resp::channel>().valueToKey(channel.toInt()), "Counter[mV]", cmd_from_host_parsing->dac_value_b_v, jsonDataHandle::dac);

             }
             else if(cmd_from_host_parsing->m_cmd == sys_cmd_resp::CMD_DAC_OUT_WORK_BIT)
             {
                 QJsonValue dac_work =dac.value("Work[bit]");

                 cmd_from_host_parsing->dac_value_a_b = static_cast<quint16>(dac_work.toInt());

                 /*Update config.txt file*/
                 Update(QMetaEnum::fromType<sys_cmd_resp::channel>().valueToKey(channel.toInt()), "Work[bit]",cmd_from_host_parsing->dac_value_a_b, jsonDataHandle::dac);

             }
             else if(cmd_from_host_parsing->m_cmd == sys_cmd_resp::CMD_DAC_OUT_COUNTER_BIT)
             {
                 QJsonValue dac_counter =dac.value("Counter[bit]");

                 cmd_from_host_parsing->dac_value_b_b = static_cast<quint16>(dac_counter.toInt());

                 /*Update config.txt file*/
                 Update(QMetaEnum::fromType<sys_cmd_resp::channel>().valueToKey(channel.toInt()), "Counter[bit]",cmd_from_host_parsing->dac_value_b_b, jsonDataHandle::dac);
             }
        }
        break;

        case sys_cmd_resp::CMD_FW_CHECK:
        break;

        case sys_cmd_resp::CMD_CHECK_AFE_CONNECTED:
        break;

        case sys_cmd_resp::CMD_BATTERY_CHECK:
        break;

        case sys_cmd_resp::CMD_UNKNOWN:
        break;
    }

    return cmd_from_host_parsing;
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

sys_cmd_resp *jsonDataHandle::check_config(sys_cmd_resp *afe_info)
{
    cmd_from_host_parsing = afe_info;

    QFile file;

    file.setFileName("/home/user/config.txt");

    if(!file.exists())
    {
        Create(afe_info->board_number);
        cmd_from_host_parsing->m_resp = sys_cmd_resp::resp::RESP_AFE_READY_FAIL;
        cmd_from_host_parsing->resp_arg[0] = sys_cmd_resp::resp_afe_fail::DAC_CONFIG_ALL_BLANK;
    }
    else
    {
        QJsonDocument jsonDoc_temp = Open(false);
        QJsonObject mainObject = jsonDoc_temp.object();
        QJsonObject ch_tmp = mainObject[QMetaEnum::fromType<sys_cmd_resp::channel>().valueToKey(afe_info->m_ch)].toObject();
        QJsonObject dac_tmp = ch_tmp["DAC"].toObject();
        QJsonObject cal_tmp = ch_tmp["Calibrarion"].toObject();

        QJsonValue dac_work_volt = dac_tmp.value("Work[mV]");
        QJsonValue dac_counter_volt = dac_tmp.value("Counter[mV]");
        QJsonValue dac_work_bit = dac_tmp.value("Work[bit]");
        QJsonValue dac_counter_bit = dac_tmp.value("Counter[bit]");

        QJsonValue cal_slope = cal_tmp.value("slope");
        QJsonValue cal_intercept = cal_tmp.value("intercept");

        Log()<<cal_slope;
        Log()<<cal_intercept;

        if((dac_work_volt.toInt() <=0)|| (dac_counter_volt.toInt() <=0) || (dac_work_bit.toInt() <=0) || (dac_counter_bit.toInt() <=0))
        {
            cmd_from_host_parsing->m_resp = sys_cmd_resp::resp::RESP_AFE_READY_FAIL;
            cmd_from_host_parsing->resp_arg[0] = sys_cmd_resp::resp_afe_fail::DAC_WORK_VOLT_VAL_BLANK;
            cmd_from_host_parsing->resp_arg[1] = sys_cmd_resp::resp_afe_fail::DAC_COUNTER_VOLT_VAL_BLANK;
            cmd_from_host_parsing->resp_arg[2] = sys_cmd_resp::resp_afe_fail::DAC_WORK_BIT_VAL_BLANK;
            cmd_from_host_parsing->resp_arg[3] = sys_cmd_resp::resp_afe_fail::DAC_COUNTER_BIT_VAL_BLANK;

            return cmd_from_host_parsing;
        }
        else
        {
            cmd_from_host_parsing->resp_arg[0] = static_cast<quint16>(dac_work_volt.toInt());
            cmd_from_host_parsing->resp_arg[1] = static_cast<quint16>(dac_counter_volt.toInt());
            cmd_from_host_parsing->resp_arg[2] = static_cast<quint16>(dac_work_bit.toInt());
            cmd_from_host_parsing->resp_arg[3] = static_cast<quint16>(dac_counter_bit.toInt());
        }

       cmd_from_host_parsing->resp_arg[4] = cal_slope.toDouble();
       cmd_from_host_parsing->resp_arg[5] = cal_intercept.toDouble();
    }

    cmd_from_host_parsing->m_resp = sys_cmd_resp::resp::RESP_AFE_READY_SUCCESS;

    return cmd_from_host_parsing;
}

sys_cmd_resp::resp jsonDataHandle::GetConfig(sys_cmd_resp *check_dac)
{
    QFile file;

    file.setFileName("/home/user/config.txt");

    if(!file.exists())
    {
         Create(board_number);

         return sys_cmd_resp::RESP_AFE_READY_FAIL;
    }

    return sys_cmd_resp::RESP_AFE_READY_SUCCESS;
}

jsonDataHandle::~jsonDataHandle()
{

}
