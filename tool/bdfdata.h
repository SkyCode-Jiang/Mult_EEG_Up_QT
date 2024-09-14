#ifndef BDFDATA_H
#define BDFDATA_H

#include <QObject>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QList>
#include <memory>
#include "extern/edflib.h"
#include "include.h"

class BdfData : public QObject
{
    Q_OBJECT
public:

    struct BdfDataStruct
    {
        QVector<QString> bdf_lab;      ///< 标签数据
        QList<int> now_label_txt_q;
        QList<int> now_label_local_q;
//        int now_label_txt[2] = {0,0};  ///< now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
        QVector<QVector<double>> EEG_BDF_data = QVector<QVector<double>>(BDF_Save,QVector<double>(SMP_FREQ,0) ) ; ///<脑电数据
    };


    BdfDataStruct bdfData;       ///< BDF数据结构
    int save_sys[BDF_Save];      ///< 保存计数用

    bool bdf_lab_flag=false;     ///< 解析获取串口数据中的ID值
    bool bdf_lab_set=false;      ///< bdf头是否设置完成标志
    int eventsave=0;             ///< 当前有几个标签需要保存
    int startsave=0;
    int MetalStart = 0;          ///< metalbci 有时会多打抱歉 多打设为1 不多打设为2


    int set_bdf();//设置BDF文件头
    void addBDFdata(int ID,int eeg_data,QVector<double> & BDF_Buff_use);
    void addlabel_zero(int id,QVector<double> & BDF_Buff_use);
    int addlabel_common(int id,QVector<double> & BDF_Buff_use_com);
    void BDF_data(int Nownum,double eeg);
    void set_Hdl(int hdl);
    int  get_Hdl(void);
    void set_Chall(int chall);
    void set_Bdf_File(QString bdf_File_Save);
    void set_MetalStart(int num);
    explicit BdfData(QObject *parent = nullptr);

//    int now_label_txt[2]={0,0}; //now_label_txt[0] 代表内容 now_label_txt[1] 代表位置

signals:

private:
    int hdl;                     //BDF句柄
    int chall;                   //通道个数
    QString bdf_File_Save;       //文件保存路径

};

#endif // BDFDATA_H
