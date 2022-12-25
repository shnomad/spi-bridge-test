#ifndef COMMON_H
#define COMMON_H
#include <QDebug>
#include <QDateTime>
#include <QObject>
#include <QThread>

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#define socket_communication_test 0

void delay_mSec(int);

#define Log() qDebug() <<"["<<QDateTime::currentDateTime().toString("MM.dd hh:mm:ss") << __PRETTY_FUNCTION__ << __LINE__ << "]"
//#define Log() qDebug()<<"["<< __PRETTY_FUNCTION__ << __LINE__ << "]"
//#define Log() qDebug(logDebug()) <<"["<< __PRETTY_FUNCTION__ << __LINE__ << "]"

using namespace std;

struct Coding_Channel_Ctl
{
    enum cmd{CMD_AFE_READY=0x60, CMD_START_MEASURE, CMD_STOP_MEASURE, CMD_DAC_OUT_VOLT, CMD_DAC_OUT_REG, CMD_DAC_CHECK, \
             CMD_FW_CHECK, CMD_CHECK_AFE_CONNECTED, CMD_BATTERY_CHECK, CMD_UNKNOWN=0xff};

    enum resp{RESP_AFE_READY_SUCCESS=0x0, RESP_START_MEASURE_SUCCESS, RESP_STOP_MEASURE_SUCCESS, RESP_DAC_OUT_SUCCESS, RESP_DAC_CHECK_SUCCESS,  \
             RESP_MEASURED_ADC_VALUE, RESP_SOCKET_CONNECTED=0x80, RESP_BATTERY_CHECK_SUCCESS, RESP_FW_CHECK_SUCCESS, \
             RESP_AFE_READY_FAIL=0xF0, RESP_START_MEASURE_FAIL, RESP_STOP_MEASURE_FAIL, RESP_DAC_OUT_WORK_FAIL, RESP_DAC_CHECK_WORK_FAIL, RESP_DAC_OUT_COUNTER_FAIL,\
             RESP_DAC_CHECK_COUNTER_FAIL, RESP_FW_CHECK_FAIL, RESP_CHECK_AFE_CONNECTED, RESP_BATTERY_CHECK_FAIL, RESP_UNKNOWN=0xff};

//  enum cmd{START_DAC = 0x10, STOP_DAC, START_ADC=0x20, STOP_ADC, UNKNOW_COMMAND = 0xff};
//  enum resp{AFE_START = 0x0, AFE_STOP , START_DAC_SUCCESS = 0x80, STOP_DAC_SUCCESS, START_ADC_SUCCESS, STOP_ADC_SUCCESS, READ_ADC, START_DAC_FAIL = 0xF0, STOP_DAC_FAIL, START_ADC_FAIL, STOP_ADC_FAIL, UNKNOW_RESPONSE = 0xff};

    enum channel{CH_001 = 0x1, CH_002, CH_003, CH_004, CH_005, CH_006, CH_007, CH_008, CH_009, CH_010, CH_011, CH_012, CH_013, CH_014, CH_015, \
                 CH_016, CH_017, CH_018, CH_019, CH_020, CH_021, CH_022, CH_023, CH_024, CH_025, CH_026, CH_027, CH_028, CH_029, CH_030, \
                 CH_031, CH_032, CH_033, CH_034, CH_035, CH_036, CH_037, CH_038, CH_039, CH_040, CH_041, CH_042, CH_043, CH_044, CH_045, \
                 CH_046, CH_047, CH_048, CH_049, CH_050, CH_051, CH_052, CH_053, CH_054, CH_055, CH_056, CH_057, CH_058, CH_059, CH_060, \
                 CH_061, CH_062, CH_063, CH_064, CH_065, CH_066, CH_067, CH_068, CH_069, CH_070, CH_071, CH_072, CH_073, CH_074, CH_075, \
                 CH_076, CH_077, CH_078, CH_079, CH_080, CH_081, CH_082, CH_083, CH_084, CH_085, CH_086, CH_087, CH_088, CH_089, CH_090, \
                 CH_091, CH_092, CH_093, CH_094, CH_095, CH_096, CH_097, CH_098, CH_099, CH_100, CH_101, CH_102, CH_103, CH_104, CH_105, \
                 CH_106, CH_107, CH_108, CH_109, CH_110, CH_111, CH_112, CH_113, CH_114, CH_115, CH_116, CH_117, CH_118, CH_119, CH_120, \
                 CH_121, CH_122, CH_123, CH_124, CH_125, CH_126, CH_127, CH_128, CH_129, CH_130, CH_131, CH_132, CH_133, CH_134, CH_135, \
                 CH_136, CH_137, CH_138, CH_139, CH_140, CH_141, CH_142, CH_143, CH_144, CH_145, CH_146, CH_147, CH_148, CH_149, CH_150, \
                 CH_151, CH_152, CH_153, CH_154, CH_155, CH_156, CH_157, CH_158, CH_159, CH_160, CH_161, CH_162, CH_163, CH_164, CH_165, \
                 CH_166, CH_167, CH_168, CH_169, CH_170, CH_171, CH_172, CH_173, CH_174, CH_175, CH_176, CH_177, CH_178, CH_179, CH_180, \
                 CH_ALL, CH_UNKNOWN=0xff};

    cmd m_cmd = cmd::CMD_UNKNOWN;
    resp m_resp = resp::RESP_UNKNOWN;
    channel m_ch = channel::CH_UNKNOWN;

    quint8 channel_number = 0x0;
    quint16 dac_value_a = 0, dac_value_b =0;
    quint16 dac_value_a_r = 0, dac_value_b_r =0;
    quint16 adc_read_value = 0x0;
    quint32 adc_interval = 0x0;
}; Q_DECLARE_METATYPE(Coding_Channel_Ctl)


struct local_network_info
{
    QString Mac_address;
    QString TCP_Local_address;
    quint16 TCP_Local_port;
    QString TCP_Remote_address;          //GUI PROGRAM
    quint16 TCP_Remote_port;             //GUI PROGRAM
    QString TCP_Remote_Host_name;
};

/*
#ifdef __cplusplus
}
#endif
*/

#endif // COMMON_H
