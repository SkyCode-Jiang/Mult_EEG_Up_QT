#include "wavemainwindow.h"
#include "ui_wavemainwindow.h"

waveMainWindow::waveMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::waveMainWindow)
{
    ui->setupUi(this);
    //ui界面初始化:
    //1.)初始化按钮组:
    groupfiltering = new QButtonGroup(this);
    groupfiltering->addButton(ui->low,0);
    groupfiltering->addButton(ui->high,1);
    groupfiltering->addButton(ui->notch,2);


    //2.)按钮组槽函数初始化:
    connect(ui->low,SIGNAL(clicked(bool)),
                this,SLOT(slots_filtering()));
    connect(ui->high,SIGNAL(clicked(bool)),
                this,SLOT(slots_filtering()));
    connect(ui->notch,SIGNAL(clicked(bool)),
                this,SLOT(slots_filtering()));


    //3.)初始化滤波文本内容:
    ui->lowEdit->setText("0");
    ui->highEdit->setText("0");
    ui->notchEdit->setText("50"); //默认值为50hz陷波
    low_f = ui->lowEdit->text().toInt();
    high_f = ui->highEdit->text().toInt();
    notch_f = ui->notchEdit->text().toInt();


    //4.)向主窗口发送信号:
    connect(ui->updata,&QPushButton::clicked,[=]()
    {
     emit this->updata_send();
    });

    connect(ui->recovery,&QPushButton::clicked,[=]()
    {
     emit this->recovery_send();
    });


}

//按钮组槽函数:
void waveMainWindow::slots_filtering()
{
    switch(groupfiltering->checkedId())
       {
       case 0:
           filtering_flag = LOWPASS_FIR;
           break;
       case 1:
           filtering_flag = HIGHPASS_FIR;
           break;
       case 2:
           filtering_flag = NOTCH_FIR;
           break;
       }
}


waveMainWindow::~waveMainWindow()
{
    delete ui;
}

//低通滤波截至频率:
void waveMainWindow::on_lowEdit_textChanged(const QString &arg1)
{
    low_f = ui->lowEdit->text().toInt();
    QString k = QString::number(low_f);
    ui->lowEdit->setText(k);
    if(low_f > max_frq)
    {
       low_f = max_frq;
       QString k = QString::number(low_f);
       ui->lowEdit->setText(k);
     }

}

//高通滤波截至频率:
void waveMainWindow::on_highEdit_textChanged(const QString &arg1)
{
    high_f = ui->highEdit->text().toInt();
    QString k = QString::number(high_f);
    ui->highEdit->setText(k);
    if(high_f > max_frq)
    {
        high_f = max_frq;
        QString k = QString::number(high_f);
        ui->highEdit->setText(k);
    }

}

//陷波
void waveMainWindow::on_notchEdit_textChanged(const QString &arg1)
{
    notch_f = ui->notchEdit->text().toInt();
    QString k = QString::number(notch_f);
    ui->notchEdit->setText(k);
    if(notch_f > max_frq)
    {
        notch_f = max_frq;
        QString k = QString::number(notch_f);
        ui->notchEdit->setText(k);
    }

}

