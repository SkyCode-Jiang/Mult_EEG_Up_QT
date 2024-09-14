#include "eegstart.h"
#include "ui_eegstart.h"


EEGStart::EEGStart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EEGStart)
{
    ui->setupUi(this);
    for (int i=0;i<MAX_Slave;i++)
    {
        Check_Num[i]=0;
    }



    ui->widget_2->setAutoFillBackground(true);
    QPalette palette = ui->widget_2->palette();
    palette.setBrush(QPalette::Window,
                     QBrush(QPixmap(":/icons/RES.png").scaled( // 缩放背景图.
                               ui->widget_2->size(),
                               Qt::IgnoreAspectRatio,
                               Qt::SmoothTransformation))); // 使用平滑的缩放方式
    ui->widget_2->setPalette(palette);

    myMbox = new MyMessageBox;

    My_Plot_Show = new MainWindow();
    My_Plot_Show->setChannelChooseFirst();
    connect(this,SIGNAL(Change_Channel(int)),My_Plot_Show,SLOT(channel_currentIndexChanged(int)));
    connect(this,SIGNAL(Change_Rate(int)),My_Plot_Show,SLOT(rate_currentIndexChanged(int)));
    connect(this,SIGNAL(Change_Com(int)),My_Plot_Show,SLOT(com_currentIndexChanged(int)));
    My_Plot_Show->hide();



    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
     {
        ui->comboBox->addItem(info.portName());
     }



}

EEGStart::~EEGStart()
{
    delete ui;
}

void EEGStart::setMaskRecog()
{
        qDebug()<<"选中了";
}



void EEGStart::on_pushButton_1_toggled(bool checked)
{

    if(Check_Num[0]==1)
    {
        Check_Num[0]=0;
        Now_Slaver_Num--;
    }
    else {
        Check_Num[0]=1;
          Now_Slaver_Num++;
    }

}

void EEGStart::on_pushButton_2_toggled(bool checked)
{

    if(Check_Num[1]==1)
    {
        Check_Num[1]=0;
        Now_Slaver_Num--;
    }
    else {
        Check_Num[1]=1;
          Now_Slaver_Num++;
    }
   // qDebug()<<"选中了2";
}

void EEGStart::on_pushButton_3_toggled(bool checked)
{
    if(Check_Num[2]==1)
    {
        Check_Num[2]=0;
        Now_Slaver_Num--;
    }
    else {
        Check_Num[2]=1;
          Now_Slaver_Num++;
    }

    qDebug()<<"选中了3";
}

void EEGStart::on_pushButton_4_toggled(bool checked)
{
    if(Check_Num[3]==1)
    {
        Check_Num[3]=0;
        Now_Slaver_Num--;
    }
    else {
        Check_Num[3]=1;
          Now_Slaver_Num++;
    }
    qDebug()<<"选中了4";
}

void EEGStart::on_pushButton_5_toggled(bool checked)
{
    if(Check_Num[4]==1)
    {
        Check_Num[4]=0;
        Now_Slaver_Num--;
    }
    else {
        Check_Num[4]=1;
          Now_Slaver_Num++;
    }
    qDebug()<<"选中了5";

}

void EEGStart::on_pushButton_6_toggled(bool checked)
{
    if(Check_Num[5]==1)
    {
        Check_Num[5]=0;
        Now_Slaver_Num--;
    }
    else {
        Check_Num[5]=1;
          Now_Slaver_Num++;
    }
    qDebug()<<"选中了6";

}

void EEGStart::on_pushButton_7_toggled(bool checked)
{
    if(Check_Num[6]==1)
    {
        Check_Num[6]=0;
        Now_Slaver_Num--;
    }
    else {
        Check_Num[6]=1;
          Now_Slaver_Num++;
    }
     qDebug()<<"选中了7";
}

void EEGStart::on_pushButton_8_toggled(bool checked)
{
    if(Check_Num[7]==1)
    {
        Check_Num[7]=0;
        Now_Slaver_Num--;
    }
    else {
        Check_Num[7]=1;
          Now_Slaver_Num++;
    }
    qDebug()<<"选中了8";

}

void EEGStart::on_comboBox_3_currentIndexChanged(int index)
{
    //连接间隔显示
    ui->comboBox_2->setCurrentIndex(index);

   // qDebug()<<index;
    emit Change_Channel(index);
}

void EEGStart::on_comboBox_4_currentIndexChanged(int index)
{
     emit Change_Rate(index);
}

void EEGStart::on_comboBox_currentIndexChanged(int index)
{
    emit  Change_Com(index);
}


void EEGStart::on_pushButton_clicked()
{
    //通道数确定
    My_Plot_Show->chall=ui->comboBox_3->currentIndex();
    qDebug()<<"chall"<< My_Plot_Show->chall;
    //波特率确定
    My_Plot_Show->rate=ui->comboBox_4->currentText();
    //数据量确定
    My_Plot_Show->shujuliangint=ui->comboBox_2->currentText().toInt();
    qDebug()<<"波特率为"<<My_Plot_Show->rate;
    qDebug()<<"数据量"<<My_Plot_Show->shujuliangint;
    My_Plot_Show->online =1;//在线实验标志
    this->hide();
    // My_Plot_Show-> My_UI_show();
    My_Plot_Show->show();
    qDebug()<<"从机数目"<<Now_Slaver_Num;
    My_Plot_Show->baseEEGStruct = new EEGData(My_Plot_Show->shujuliangint,My_Plot_Show->chall);
    qDebug()<<"初始化基本结构体"<<My_Plot_Show->shujuliangint<< "   "<<My_Plot_Show->chall;
}





void EEGStart::on_comboBox_3_activated(const QString &arg1)
{

}


void EEGStart::on_pushButton_21_clicked()
{

}
