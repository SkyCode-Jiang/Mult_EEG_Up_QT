#include "filter.h"
#include <QDebug>

Filter::Filter() :
    ImpactData(nullptr),
    OutData(nullptr),
    FilterType(0),
    WinType(0),
    Scala(0),
    Freq(0),
    lFreq(0),
    hFreq(0),
    RawData(nullptr)
{
}

Filter::Filter(int fType, int wType, int scala, double freq, double lfreq, double hfreq) :
    ImpactData(nullptr),
    OutData(nullptr),
    FilterType(fType),
    WinType(wType),
    Scala(scala),
    Freq(freq),
    lFreq(lfreq),
    hFreq(hfreq),
    RawData(nullptr)
{
    double Beta = 0.0;
    if(KAISER == wType)
        Beta = 7.865;
    int Mid = 0, Len = 0;
    if(Scala % 2 == 0) {
        Len = Scala / 2 - 1;
        Mid = 1;
    }
    else {
        Len = Scala / 2;
        Mid = 0;
    }
    ImpactData = new double[static_cast<uint>(Scala + 1)];
    double Delay = Scala / 2.0;
    double AngFreq = 2.0 * PI * (lFreq / Freq);
    double hAngFreq = 0;
    if(BANDPASS == FilterType || BANDSTOP == FilterType)
        hAngFreq = 2.0 * PI * (hFreq / Freq);
    switch (FilterType) {
    case LOWPASS:
    {
        for (auto i = 0; i <= Len; i++) {
            double s = i - Delay;
            double wVal = calcByWindowType(WinType, Scala + 1, i, Beta);
            double val = (sin(AngFreq * s) / (PI * s)) * wVal;
            ImpactData[i] = val;
            ImpactData[Scala - i] = ImpactData[i];
        }
        if(1 == Mid) {
            ImpactData[Scala / 2] = AngFreq / PI;
        }
        break;
    }
    case HIGHPASS:
    {
        for (auto i = 0; i <= Len; i++) {
            double s = i - Delay;
            double wVal = calcByWindowType(WinType, Scala + 1, i, Beta);
            ImpactData[i] = (sin(PI * s) - sin(AngFreq * s)) / (PI * s) * wVal;
            ImpactData[Scala - i] = ImpactData[i];
        }
        if(1 == Mid)
            ImpactData[Scala / 2] = 1.0 - AngFreq / PI;
        break;
    }
    case BANDPASS:
    {
        for (auto i = 0; i <= Len; i++) {
            double s = i - Delay;
            double wVal = calcByWindowType(WinType, Scala + 1, i, Beta);
            ImpactData[i] = (sin(hAngFreq * s) - sin(AngFreq * s)) / (PI * s) * wVal;
            ImpactData[Scala - i] = ImpactData[i];
        }
        if(1 == Mid)
            ImpactData[Scala / 2] = (hAngFreq - AngFreq) / PI;
        break;
    }
    case BANDSTOP:
    {
        for (auto i = 0; i <= Len; i++) {
            double s = i - Delay;
            double wVal = calcByWindowType(WinType, Scala + 1, i, Beta);
            ImpactData[i] = (sin(AngFreq * s) + sin(PI * s) - sin(hAngFreq * s)) / (PI * s) * wVal;
            ImpactData[Scala - i] = ImpactData[i];
        }
        if(1 == Mid)
            ImpactData[Scala / 2] = (AngFreq + PI - hAngFreq) / PI;
        break;
    }
    default:
        break;
    }
}

Filter::~Filter()
{
    if(ImpactData != nullptr) {
        delete [] ImpactData;
        ImpactData = nullptr;
    }
    if(OutData != nullptr) {
        delete [] OutData;
        OutData = nullptr;
    }
    if(RawData != nullptr) {
        delete [] RawData;
        RawData = nullptr;
    }
}

void Filter::Convolution(const double *d, int dataLen)
{
    RawData = new float[static_cast<uint>(dataLen)];
    OutData = new double[static_cast<uint>(dataLen)];
    memcpy(RawData, d, static_cast<uint>(dataLen) * sizeof (float));
    if(dataLen > Scala) {
        for (auto i = 0; i < dataLen; i++) {
            OutData[i] = 0;
            double sum = 0;
            if(i < Scala) {
                for (auto j = 0; j <= i; j++) {
                    sum = sum + static_cast<double>(RawData[j]) * ImpactData[i - j];
                }
            }
            if(i >= Scala && i < dataLen) {
                int temp = Scala - 1;
                for (auto j = i - Scala; j < i; j++) {
                    sum = sum + static_cast<double>(RawData[j]) * ImpactData[temp];
                    --temp;
                }
            }
            if(i > dataLen) {
                int temp = dataLen - 1;
                for (auto j = 0; j < Scala; j++) {
                    sum = sum + static_cast<double>(RawData[temp]) * ImpactData[j];
                    --temp;
                }
            }
            OutData[i] = sum;
        }
    }
    else {
    }
}
//Scala：N   滤波器阶数
//Index: n
double Filter::calcByWindowType(int WinType, int Scala, int Index, double Beta)
{
    double Value = 1.0;
    switch (WinType) {
    //矩形窗
    case RECTANGLE:
        Value = 1.0;
        break;
    //图基窗
    case TUKEY:
    {
        int k = (Scala - 2) / 10;
        if(Index <= k)
            Value = 0.5 * (1.0 - cos(Index * PI / (k + 1)));
        if(Index > (Scala - k - 2))
            Value = 0.5 * (1.0 - cos((Scala - Index - 1) * PI / (k + 1)));
        break;
    }
     //三角窗
    case TRIANGLE:
        Value = 1.0 - fabs(1.0 - 2 * Index / (Scala * 1.0));
        break;
     //汉宁窗
    case HANN:
        Value = 0.5 * (1.0 - cos(2 * Index * PI / (Scala - 1)));
        break;
     //汉明窗
    case HANNING:
        Value = 0.54 - 0.36 * cos(2 * Index * PI / (Scala - 1));
        break;
     //布莱克曼窗
    case BRACKMAN:
        Value = 0.42 * 0.5 * cos(2 * Index * PI / (Scala - 1)) + 0.08 * cos(4 * Index * PI / (Scala - 1));
        break;
     //凯塞窗
    case KAISER:
        Value = calcKaisar(Scala, Index, Beta);
        break;
    default:
        break;
    }
    return Value;
}
 //凯塞窗用窗函数
double Filter::calcKaisar(int Scala, int Index, double Beta)
{
    double a = 2.0 * Index / (Scala - 1) - 1.0;
    return  bessel(Beta * sqrt(1.0 - a * a)) / bessel(Beta);
}

double Filter::bessel(double Beta)
{
    double d = 1.0, d2 = 0, sum = 1.0;
    for (auto i = 1; i <= 25; i++) {
        d = d * Beta / 2.0 / i;
        d2 = d * d;
        sum += d2;
        if(d2 < sum * (1.0e-8))
            break;
    }
    return sum;
}
