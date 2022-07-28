#ifndef ADS8866_H
#define ADS8866_H

#include <QObject>

class ads8866 : public QObject
{
    Q_OBJECT
public:
    explicit ads8866(QObject *parent = nullptr);    
    float  CalAdcValue(quint8 *);
    ~ads8866();

signals:

public slots:
};

#endif // ADS8866_H
