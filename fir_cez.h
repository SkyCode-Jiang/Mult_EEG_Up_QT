#ifndef FIR_CEZ_H
#define FIR_CEZ_H

#include <complex>
#include <QVector>
#define TWOPI 6.2831852
#define PI 3.1415926

class fir_cez
{
    public:
    int test = 0;
    int filtType; // 1:low ; 2:heigh ; 3:notch
    double wstop1; //低通和高通的阻带边缘频率
    double wpass1; //低通和高通的通带边缘频率
    double wstop2;
    double wpass2;
    int order = 0; //阶数
    double fsamp = 1000; //采样频率
    QVector<double> coefs; //系数
    QVector<double> wind;//窗函数

    QVector<double> blackman(int N, double amp ); //布莱克曼窗:N为系数，amp为增益
    void setParams( double fs, int N ,int flag); //参数设置: fs为阻带边缘频率，N为阶数,flag为滤波类型
    void calcCoef(); //计算FIR的系数
    void idealCoef(); //计算FIR理想滤波器系数
    void design(double fs, int N ,int flag);
};

QVector<double> fir_cez :: blackman( int N, double amp )
{
    QVector<double> win(N);

    for( int i=0; i<(N+1)/2; ++i )
    {
        win[i] = amp * double ( 0.42 - 0.50*cos(TWOPI*i/(N-1.0))
                              + 0.08*cos(2*TWOPI*i/(N-1.0)) );
        win[N-1-i] = win[i];
    }

    return win;
}

void fir_cez :: setParams(double fs, int N, int flag)
{
    wstop1 = fs;
    wpass1 = wstop1 - 11*PI/N;
    filtType = flag;
    wpass1 = wpass1 * TWOPI;
    wstop1 = wstop1 * TWOPI;
    order = N;

    wstop2 = fs+5;
    wpass2 = wstop2 - 11*PI/N;
    wstop2 = wstop2 * TWOPI;
    wpass2 = wpass2 * TWOPI;
}

void fir_cez::idealCoef()
{
    int     i;
    double  t,
            tau,
            Wc1,
            Wc2;

    // Calculate tau as non-integer if order is even.
    tau =  ( order - 1 ) / 2.0;

    // Adjust cutoff frequency to midway point between stop
    // and pass edge frequency and convert to digital frequency.
    Wc1 = (wstop1 + wpass1) / (2*fsamp);
    Wc2 = (wstop2 + wpass2) / (2*fsamp);

    // Calc coefs based on selectivity of filter.
    if( filtType == 1)
        for( i=0; i<order; ++i )
        {
            if( i == tau )
                coefs[i] = Wc1/PI;
            else
            {
                t = i - tau;
                coefs[i] = sin(Wc1*t) / (PI*t);
            }
        }

    else if( filtType == 2 )
        for( i=0; i<order; ++i )
        {
            if( i == tau )
                coefs[i] = (PI-Wc1) / PI;
            else
            {
                t = i - tau;
                coefs[i] = ( sin(PI*t) - sin(Wc1*t) ) / (PI*t);
            }
        }

    else if (filtType == 3)
    {
        for( i=0; i<order; ++i )
        {
            if( i == tau )
                coefs[i] = ( PI+Wc1-Wc2 ) / PI;
            else
            {
                t = i - tau;
                coefs[i] = ( sin(PI*t)-sin(Wc2*t)+sin(Wc1*t) ) / (PI*t);
            }
        }
    }
}


void fir_cez :: calcCoef()
{
    coefs.resize(order); //系数
    wind.resize(order); //窗函数
    idealCoef(); //理想滤波器系数
    wind = blackman(order, 1); //窗函数系数

    //理想滤波器加窗
    for(auto i =0; i<order; ++i)
    {
        coefs[i] = coefs[i] * wind[i];
    }
}

void fir_cez :: design(double fs, int N ,int flag)
{
    setParams(fs,N,flag);
    //生成FIR滤波器
    calcCoef();

}


#endif // FIR_CEZ_H
