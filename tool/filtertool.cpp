#include "filtertool.h"

FilterTool::FilterTool(QObject *parent) : QObject(parent)
{
    for (int i = 0; i < 19; ++i) {
        fir[i] = nullptr;
        notch_fir[i] = nullptr;
    }
}
/**********************************************
* @projectName   EEGplylot
* @brief         滤波器设计函数
* @param         低通截止频率、
*                高通截止频率、
*                陷波截至频率、
*                陷波截至频率、
*                类型、
*                阶数、
*                窗函数、
* @return        void
* @author        JCLStill
* @date          2024-07-08
**********************************************/
void FilterTool:: setFilter(double low_freq,double high_freq,double notch_lowfreq,double notch_highfreq,int fir_order,int fir_type)
{
    if(fir_type == 1)
    {
        double low_freq_1=low_freq/SAMPLE;
        for(int i=0;i<19;i++)
        {
            fir[i] = new FIR_filter(fir_order,low_freq_1,0,"lp","hamming");
        }
        qDebug()<<"低通滤波器设计完成"<<"阶数"<<fir_order<<"低通截止频率"<<low_freq_1;
    }
    if(fir_type == 2)
        //高通滤波器
    {
        double high_freq_1=high_freq/SAMPLE;
        for(int i=0;i<19;i++)
        {
            fir[i] = new FIR_filter(fir_order,high_freq_1,0,"hp","hamming");
        }
        qDebug()<<"高通滤波器设计完成";
    }

    if(fir_type == 3)
    {
        //陷波滤波器
        double notch_lowfreq_3=notch_lowfreq/SAMPLE;
        double notch_highfreq_3=notch_highfreq/SAMPLE;
        for(int i=0;i<19;i++)
        {
            notch_fir[i] = new FIR_filter(fir_order,notch_lowfreq_3,notch_highfreq_3,"sb","hamming");
        }
        qDebug()<<"陷波滤波器设计完成";
    }
    if(fir_type == 4)
    {
        //带通滤波器
        double low_freq_2=low_freq/SAMPLE;
        double high_freq_2=high_freq/SAMPLE;
        for(int i=0;i<19;i++)
        {
            fir[i] = new FIR_filter(fir_order,low_freq_2,high_freq_2,"bp","hamming");
        }
        qDebug()<<"带通滤波器设计完成";

    }
}
