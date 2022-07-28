#include "ads8866.h"

ads8866::ads8866(QObject *parent) : QObject(parent)
{

}

float ads8866::CalAdcValue(quint8 *adc_value)
{
    quint16 digit_value =0;
    float result = 0;

    float Vref = 3000.0f;
//  float Voffset = 0.64f;

    digit_value |= adc_value[0] << 8;
    digit_value |= adc_value[1];

    result = digit_value * (Vref/65535.0f);

    return result;
}

ads8866::~ads8866()
{

}
