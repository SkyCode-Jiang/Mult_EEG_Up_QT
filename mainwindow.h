#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPortInfo>
#include <QDebug>
#include "extern/qcustomplot.h"
#include <complex>
#include <QTextStream>
#include "ui/FilterConfigWidget/wavemainwindow.h" //波形设置界面头文件
#include "ui/MessboxWidget/mymessagebox.h"
#include "ui/EEGDataPlotWidget/qttcustomplot.h"
#include "extern/edflib.h"
//TCP数据
#include <QTcpServer>
#include <QTcpSocket>
//#include "filter.h"
#include "tool/filtertool.h"
#include "tool/bdfdata.h"
#include "device/devicemicroeeg.h"
#include "datastruct/eegdata.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
//最大从机数
//#define MAX_Slave 20
//#define BDF_Save 21

#define AppFontName "Microsoft YaHei"
#define AppFontSize 9
#define TextColor QColor(255,255,255)
#define TextWidth 1
//#define Single_Show 0
//#define Mult_Show 1
#define NOW_IMP_SHOW 0
#define NOW_EEG_SHOW 1

#define  SAMPLE 1000

//bdf文件存储
//#define SMP_FREQ  (1000)

//txt分包存储 20M
//#define SUB_SAVE
#define EEG_CONN 1
#define IMP_CONN 2
#define ColStep0_20 "rgb(25,25,112)"
#define ColStep20_40 "rgb(65,105,225)"
#define ColStep40_60 "rgb(100,149,237)"
#define ColStep60_80 "rgb(255,182,193)"
#define ColStep80_100 "rgb(255,20,147)"
#define ColStepmore100 "rgb(220,20,6)"

//#define ONLINENUM 8


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    waveMainWindow *wavepage = NULL; //定义波形设置界面类
    QThread *thread1;
    QTextStream out;
    QFile pathx;



    //在线数据发送
    int online =0;//在线实验标志
    int onlien_startsend = 0;
    void Onlie_SendData_add(QString EEG_data,int EEG_Event);
    void Onlie_Send(QString Now_Data);
    void Onlie_Use_Dada(QString Onlie_UseDada);
    int Now_TCP_Data = 0;
    int  Onlie_SendEvent = 0;
    int  Onlie_NowEvent = 0;
    int  Onlie_DoneEvent = 0;
    int  Online_Num = 0;


    QVector<QString> Online_SendData;
    QString Onlie_SendTCPData;


     int now_label_txt[2]={0,0}; //now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
     int bdf_save_label[2]={0,0};
    void ALLdata_first_set();  //根据从机数目初始化使用到的数组

   //阻抗检测相关
    int EEG_IMP_Function = EEG_CONN;
    double LAST_Implace=0.0;
    //阻抗颜色
     QVector<QString> Allcol={"rgb(0,0,0)","rgb(25,25,112)","rgb(65,105,225)","rgb(255,182,193)","rgb(255,20,147)","rgb(220,20,6)"};

    MyMessageBox *Imp10k;
    int IMPPD=0;


    void setChannelChooseFirst();///< 要关联UI的channel下拉框



private slots:
    void on_serial_clicked();

    void on_com_clicked();

    void on_file_clicked();

    void on_save_clicked();

    void on_r_g_clicked();

    void on_rate_currentIndexChanged(const QString &arg1);

    void on_com_currentIndexChanged(const QString &arg1);

    void on_channel_currentIndexChanged(const QString &arg1);

    void on_star_clicked();

    void on_initialization_clicked();

    void on_x_down_clicked();

    void on_x_up_clicked();

    void on_y_set_currentIndexChanged(const QString &arg1);

    void on_wave_clicked();

    void on_lineEdit_2_textChanged(const QString &arg1);

    void channel_currentIndexChanged(int index);

    void rate_currentIndexChanged(int index);

     void com_currentIndexChanged(int index);


     void on_pushButton_3_clicked();
     void onNewConnect();
     void onDisConnect();
     void onReadData();

     void on_pushButton_4_clicked();

     void on_pushButton_5_clicked();

     void on_pushButton_6_clicked();

     void on_pushButton_7_clicked();

public:
    void displot_intial();//显示屏幕初始化
    void Create_canvas(double num);//创建画布
    void displayData(); //串口接受线程
    void filtering_muli(); //多通道滤波函数
    void recovery_muli();//恢复至原始波形

    //TCP在线相关
    QTcpServer  *tcpServer;//监听套接字
    QTcpSocket  *tcpSocket;//通信套接字



    bool serial_flag = false; //串口开关
    bool save_flag = false; //文件保存标志位
    bool star_flag = false; //显示暂停
    bool show_model = false; //显示模式
    bool save_path = false; //保存路径标签
    bool y_rflag = false; //初始化标志位
    QDesktopWidget topweight;//获取屏幕分辨率
    QCPItemStraightLine *m_vline; //显示游标初始化
    int x_range = 5; //x轴显示范围
    double y_range_mm = 750;//y轴显示范围(mm版本)
    double y_range2_mm = 1500;//y显示范围中继函数(mm版本)
    QVector<QPen> colour;//线条颜色
    QVector<double> V_constant;//初始值显示_常数
    double variable_y = 0;
    QVector<double> ch1; //通道存储
    QVector<double> ch[MAX_Slave]={}; //通道存储

    int test_flag;
    int flag_bb = 0;//数据头寻找
    int flag_end =0;//数据尾寻找
    int bb_num = 0;
    QVector<QString> framedata_al;  ///<串口接受到的数据
   //  std::unique_ptr<QVector<QString>> framedata_al;

  //  QString framedata;
    QString cha_data;
    int first_flag = 0;
    int count_num = 0;
    int stop_flag = 0;
    bool flag_y = true;

    QString File_save; //文件保存路径
    QString File_save_change; //分包存储文件保存路径
    double Suf=0;
    int File_save_suf=1;
    QString rate; //波特率
    QString com; //端口号
    QString channel; //通道数
    int chall = 0; //通道数
    int chall_num = 0;
    double Max=-1.79e308;
    double Min=1.79e308;
    double MAX_Shou[MAX_Slave];
    double MIN_Shou[MAX_Slave];
    //数据处理相关
    double timerx = 0;
    double time_x[MAX_Slave];

    bool OK;
    double pen = -100000; //测试用，正式版删除
    double self_adaption_a = 0; //y轴显示系数
    double self_adaption_b = 0; //y轴显示常数

    //标签显示相关
    void createVLineTag2(QString val, int x, double y);
    void createVLineTag3(QString val, int x, double y, int num);
    QString cha_lab; //承接标签位置
    QVector<QCPItemText*> all_lab;
    QVector<QCPItemStraightLine*> all_linex;

    //校验丢包相关
    int last_save[MAX_Slave];
    int now_save[MAX_Slave];
    int last_save_signel = 0;//纪录上次保存
    void data_verification(QString rec);//数据校验
    int last_save_flag = 0;
    int path_long = 0;
    int loss = 0; //丢包个数

    //test---正式版本需要删除
    void dis_test(double num);//通道测试程序，正式版本需要删除
    //ui->lineEdit_2 正式版本需要删除

    //新格式多通道数据设置
    void data_sample();//多通道格式设置

    void chuli_muli_new_fir(QString rec, int L);


    void data_verification_new(int *now,int* last);
    void data_verification_all(QString head,QString rec);



    int shujuliangint = 25;//数据量
    //fir滤波相关
    int fir_num = 0; //卷积的个数
    bool fir_flag = false; ///< 滤波标志
    QVector<double> fir_wind;
    QVector<double> fir_all[8]={}; //暂时存储数据
    QVector<double> fir_temp;
    bool fir_start = false;
    void top_data(); //数据头和数据尾平滑处理

    MyMessageBox *Show_Mes;
    int init_pd_flag=0;
    int conn_pd_flag=0;
    int disconn_pd_flag=0;
    QString Dis_Situation;
    QString disconn_slaver;
    void initAnay(QString str);
    void connAnay(QString str);
    void disconnAnay(QString str);
    void DisConAnay(QString str);
    void dataUartAnny(QByteArray rx_data);
    void Calibration_value(int Chall_show_value,double unint_mv);
    int LabAnary(QString str);                                     //标签事件处理

    QttCustomPlot *mPlot;
    void My_UI_show();
    int Fir_Star_Con=0;
    int  legset=0;//初始化图例
    int Now_Function = NOW_EEG_SHOW;
    int  show_vale[MAX_Slave];//Y轴定标用
    int save_size=2072;


    void lable_order(QCustomPlot *my_customplot,QCPAxis *yAxis, QString label, QCPAxisRect *rect);
    QCPItemText* yAxisLabel[16];

    void plot_EEG_Fun(double eeg,int i_num,int now_channel);
    void plot_IMP_Fun(double eeg,int i_num,int now_channel);

private:
    Ui::MainWindow *ui;

    BdfData *myBdfData;         ///< BDF保存类
    FilterTool *myFilterTool;   ///< 滤波器工具类
    DeviceMicroEEG *myMicroEEG; ///< 串口EEG设备
public:
        EEGData *baseEEGStruct;




};
#endif // MAINWINDOW_H
