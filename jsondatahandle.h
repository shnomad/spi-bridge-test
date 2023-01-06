#ifndef JSONDATAHANDLE_H
#define JSONDATAHANDLE_H

#include <QObject>
#include <QFile>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "common.h"

class jsonDataHandle : public QObject
{
    Q_OBJECT

    static const QString filename;

public:
    explicit jsonDataHandle(QObject *parent = nullptr);
//    explicit jsonDataHandle(quint8, QObject *parent = nullptr);
    ~jsonDataHandle();

    QString encode_resp(Coding_Channel_Ctl, quint16 *);
    Coding_Channel_Ctl parse(QString);

    enum update_sel
    {
        dac =0x0,
        cal
    };

    bool Create(quint8);

    QMutex mutex;

signals:

public slots:

private:
    Coding_Channel_Ctl ch_ctl_cmd{};    

    /*set&get config.json*/    
    QJsonDocument Open(const bool is_write = false);
    bool Update(QString channel,QString key_, quint16 value_, update_sel);
    quint8 board_number;

    const QString total_channel[12][15] ={\
             {"CH_001", "CH_002", "CH_003", "CH_004", "CH_005", "CH_006", "CH_007", "CH_008", "CH_009", "CH_010", "CH_011", "CH_012", "CH_013", "CH_014", "CH_015"}, \
             {"CH_016", "CH_017", "CH_018", "CH_019", "CH_020", "CH_021", "CH_022", "CH_023", "CH_024", "CH_025", "CH_026", "CH_027", "CH_028", "CH_029", "CH_030"}, \
             {"CH_031", "CH_032", "CH_033", "CH_034", "CH_035", "CH_036", "CH_037", "CH_038", "CH_039", "CH_040", "CH_041", "CH_042", "CH_043", "CH_044", "CH_045"}, \
             {"CH_046", "CH_047", "CH_048", "CH_049", "CH_050", "CH_051", "CH_052", "CH_053", "CH_054", "CH_055", "CH_056", "CH_057", "CH_058", "CH_059", "CH_060"}, \
             {"CH_061", "CH_062", "CH_063", "CH_064", "CH_065", "CH_066", "CH_067", "CH_068", "CH_069", "CH_070", "CH_071", "CH_072", "CH_073", "CH_074", "CH_075"}, \
             {"CH_076", "CH_077", "CH_078", "CH_079", "CH_080", "CH_081", "CH_082", "CH_083", "CH_084", "CH_085", "CH_086", "CH_087", "CH_088", "CH_089", "CH_090"}, \
             {"CH_091", "CH_092", "CH_093", "CH_094", "CH_095", "CH_096", "CH_097", "CH_098", "CH_099", "CH_100", "CH_101", "CH_102", "CH_103", "CH_104", "CH_105"}, \
             {"CH_106", "CH_107", "CH_108", "CH_109", "CH_110", "CH_111", "CH_112", "CH_113", "CH_114", "CH_115", "CH_116", "CH_117", "CH_118", "CH_119", "CH_120"}, \
             {"CH_121", "CH_122", "CH_123", "CH_124", "CH_125", "CH_126", "CH_127", "CH_128", "CH_129", "CH_130", "CH_131", "CH_132", "CH_133", "CH_134", "CH_135"}, \
             {"CH_136", "CH_137", "CH_138", "CH_139", "CH_140", "CH_141", "CH_142", "CH_143", "CH_144", "CH_145", "CH_146", "CH_147", "CH_148", "CH_149", "CH_150"}, \
             {"CH_151", "CH_152", "CH_153", "CH_154", "CH_155", "CH_156", "CH_157", "CH_158", "CH_159", "CH_160", "CH_161", "CH_162", "CH_163", "CH_164", "CH_165"}, \
             {"CH_166", "CH_167", "CH_168", "CH_169", "CH_170", "CH_171", "CH_172", "CH_173", "CH_174", "CH_175", "CH_176", "CH_177", "CH_178", "CH_179", "CH_180"} \
    };

    const QString total_channel_1d[180];

};

#endif // JSONDATAHANDLE_H
