#ifndef FILTER_H
#define FILTER_H
#include <math.h>

#define     PI      (3.1415926)
#define     DPI     (2.0 * PI)

class Filter
{
public:
    Filter();
    /* fType:过滤器类型
     * wType:窗口类型
     * scala:阶数
     * freq:频率
     * lfreq:下限频率
     * hfreq:上限频率
     */
    Filter(int fType, int wType, int scala, double freq, double lfreq, double hfreq);
    ~Filter();
    // 获得滤波器系数后,对输入信号和系数做卷积积分,得到滤波器输出,d:原始数据，dataLen:数据长度
    void Convolution(const double *d, int dataLen);
    // 滤波器类型
    enum FILTERTYPE{
        LOWPASS,        // 低通
        HIGHPASS,       // 高通
        BANDPASS,       // 带通
        BANDSTOP        // 带阻
    };
    // 窗口类型
    enum WINDOWTYPE {
        RECTANGLE,      // 矩形
        TUKEY,          // 图基
        TRIANGLE,       // 三角
        HANN,           // 汉宁
        HANNING,        // 汉明
        BRACKMAN,       // 布莱克曼窗
        KAISER          // 凯塞
    };
protected:
    //窗函数
    double calcByWindowType(int WinType, int Scala, int Index, double Beta);
    double calcKaisar(int Scala, int Index, double Beta);
    double bessel(double Beta);
public:
    double *ImpactData;     // 冲击响应数据
    double *OutData;
private:
    int FilterType;
    int WinType;
    int Scala;
    double Freq;
    double lFreq;
    double hFreq;
    float *RawData;
};

#endif // FILTER_H
