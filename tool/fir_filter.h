#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <stdlib.h>
#include <cmath>
#include <string.h>
#include <string.h>
#include <vector>
#include <QString>

class FIR_filter
{
public:
    FIR_filter( int taps=0, double f1=0, double f2=0, QString type="",
                QString window="");
    ~FIR_filter();

    std::vector<double> getCoefficients();  //获取滤波器系数

    double filter(double new_sample);

private:
    //获取滤波器系数
    std::vector<double> lowPass_coefficient( int taps, double f);
    std::vector<double> highPass_coefficient(int taps, double f);
    std::vector<double> bandPass_coefficient(int taps, double f1, double f2);
    std::vector<double> bandStop_coefficient(int taps, double f1, double f2);
    //设计窗函数
    std::vector<double> window_hammig(int taps);
    std::vector<double> window_triangle(int taps);
    std::vector<double> window_hanning(int taps);
    std::vector<double> window_blackman(int taps);

    std::vector<double> h;       // FIR 滤波器系数
    std::vector<double> samples; // FIR 输入信号缓冲区

    int idx;        // Round robin index
    int taps;       ///< 滤波器阶数
};


#endif // FIR_FILTER_H
