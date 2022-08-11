#ifndef JSONDATAHANDLE_H
#define JSONDATAHANDLE_H

#include <QObject>

class jsonDataHandle : public QObject
{
    Q_OBJECT
public:
    explicit jsonDataHandle(QObject *parent = nullptr);
    ~jsonDataHandle();

    QString create(quint16, quint16, float, float);
    QJsonObject parse(QString);

signals:

public slots:
};

#endif // JSONDATAHANDLE_H
