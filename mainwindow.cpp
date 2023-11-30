#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <QTextStream>
#include "wavemainwindow.h" //波形设置界面头文件
#include "fir_cez.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thread1 = new QThread();
    ALLdata_first_set();
    ui->file->setDisabled(1);
    ui->save->setDisabled(1);
  //  thread2 = new QThread();

    mPlot = new QttCustomPlot(1);
    //设置自动滚动
    mPlot->setAutoScroll(false);
    mPlot->setLegendVisible(true);

    ui->gridLayout->addWidget(mPlot);


    //波信设置界面实例化:
    this->wavepage = new waveMainWindow;
    //接受updata发来的消息:
    connect(this->wavepage,&waveMainWindow::updata_send,[=]()
    {
        filtering_muli();
    });

    connect(this->wavepage,&waveMainWindow::recovery_send,[=]()
    {
        recovery_muli();
    });

    //ui界面初始化:
    int pri = topweight.primaryScreen(); //获取屏幕编号
    int h = topweight.screenGeometry(pri).height();//获得屏幕的高
    int w = topweight.screenGeometry(pri).width();//获得屏幕的宽
    ui->show->setMinimumSize(w, h-300);//自适应屏幕分辨率

    ui->gridLayoutWidget->setMinimumSize(w,h-200);
    ui->stackedWidget->setMinimumSize(w,h-200);
    ui->stackedWidget->setCurrentIndex(0);



    showMaximized();//全屏显示
    //ui->show->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);//模式：框选放大
    //视图一画笔
//    colour.append(QPen(Qt::magenta));//ch1
//    colour.append(QPen(Qt::red)); //ch2
//    colour.append(QPen(Qt::darkGreen)); //ch3
//    colour.append(QPen(Qt::cyan)); //ch4
//    colour.append(QPen(Qt::yellow)); //ch5
//    colour.append(QPen(Qt::green)); //ch6
//    colour.append(QPen(Qt::white)); //ch7
//    colour.append(QPen(Qt::lightGray)); //ch8

//    colour.append(QPen(Qt::black));//ch1
//    colour.append(QPen(Qt::gray)); //ch2
//    colour.append(QPen(Qt::darkGray)); //ch3
//    colour.append(QPen(Qt::blue)); //ch4
//    colour.append(QPen(Qt::darkRed)); //ch5
//    colour.append(QPen(Qt::darkBlue)); //ch6
//    colour.append(QPen(Qt::darkCyan)); //ch7
//    colour.append(QPen(Qt::darkMagenta)); //ch8
//    colour.append(QPen(Qt::darkYellow));//ch1

    colour.append(QPen(QColor(255,128,128))); //ch2
    colour.append(QPen(QColor(255,0,0))); //ch3
    colour.append(QPen(QColor(0,255,0))); //ch4
    colour.append(QPen(QColor(255,255,255))); //ch2
    colour.append(QPen(QColor(255,255,0))); //ch3

    colour.append(QPen(QColor(0,64,0))); //ch2
    colour.append(QPen(QColor(255,128,0))); //ch3
    colour.append(QPen(QColor(128,128,0))); //ch4
    colour.append(QPen(QColor(128,255,255))); //ch2
    colour.append(QPen(QColor(0,64,128))); //ch3

    colour.append(QPen(QColor(0,0,255))); //ch2
    colour.append(QPen(QColor(0,128,255))); //ch3
    colour.append(QPen(QColor(0,128,192))); //ch4
    colour.append(QPen(QColor(128,128,255))); //ch2
    colour.append(QPen(QColor(192,192,192))); //ch3

    colour.append(QPen(QColor(255,128,192))); //ch2
    colour.append(QPen(QColor(128,0,64))); //ch3
    colour.append(QPen(QColor(255,0,255))); //ch4
    colour.append(QPen(QColor(128,0,255))); //ch2
    colour.append(QPen(QColor(79,79,79))); //ch3



    //(1. 串口打开or关闭:
    ui->serial->setIconSize(QSize(50,50));
    ui->serial->setIcon(QIcon(":/break off.png"));

    //(2. 波特率设置:
   // rate = "115200";
    rate = "3000000";
    ui->rate->addItem("3000000");//调试用的默认值
    ui->rate->addItem("1200");
    ui->rate->addItem("2400");
    ui->rate->addItem("4800");
    ui->rate->addItem("9600");
    ui->rate->addItem("19200");
    ui->rate->addItem("38400");
    ui->rate->addItem("36400");
    ui->rate->addItem("57600");
    ui->rate->addItem("115200");

    //(3. 串口端口设置:
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
     {
        ui->com->addItem(info.portName());
     }
     connect(&this->mySer, &QSerialPort::readyRead , this, &MainWindow::displayData); //串口连接槽函数

    //(4. 信号岛通道设置:
    ui->channel->addItem("ch1");
    ui->channel->addItem("ch2");
    ui->channel->addItem("ch3");
    ui->channel->addItem("ch4");
    ui->channel->addItem("ch5");
    ui->channel->addItem("ch6");
    ui->channel->addItem("ch7");
    ui->channel->addItem("ch8");
    ui->channel->addItem("ch9");
    ui->channel->addItem("ch10");
    ui->channel->addItem("ch11");
    ui->channel->addItem("ch12");
    ui->channel->addItem("ch13");
    ui->channel->addItem("ch14");
    ui->channel->addItem("ch15");
    ui->channel->addItem("ch16");
    ui->channel->addItem("ch17");
    ui->channel->addItem("ch18");
    ui->channel->addItem("ch19");
    ui->channel->addItem("ch20");


    //(5. 显示窗口初始化:
    ui->show->xAxis->setRange(0, x_range);//修改x的显示范围
    ui->show->yAxis->setRange(-y_range_mm, y_range_mm);//修改y的显示范围

//    ui->show->yAxis->setTickLabels(false); //删除y轴
//    ui->show->yAxis->grid()->setVisible(false);//不显示y轴网格
    ui->show->axisRect()->setupFullAxesBox();
    //ui->show->setInteractions
            //(QCP::iRangeDrag|QCP::iRangeZoom|QCP::iSelectPlottables);//可以缩放可以滚动点击选择
    ui->show->setInteractions
            (QCP::iRangeDrag|QCP::iRangeZoom);//可以缩放可以滚动点击选择
    m_vline = new QCPItemStraightLine(ui->show);
    m_vline->setLayer("overlay");
    m_vline->setPen(QPen(Qt::blue));
    m_vline->setClipToAxisRect(true);
    m_vline->point1->setCoords(0, 0);
    m_vline->point2->setCoords(0, 1);
    displot_intial();

    //(6. y轴调整:
    //    ui->y_set->addItem("0.0001");
    //    ui->y_set->addItem("0.0005");
    //    ui->y_set->addItem("0.001");
    //    ui->y_set->addItem("0.005");
    //    ui->y_set->addItem("0.01");
    //    ui->y_set->addItem("0.05");
    //    ui->y_set->addItem("0.1");
    ui->y_set->addItem("0.1");
    ui->y_set->addItem("1");
    ui->y_set->addItem("2");
    ui->y_set->addItem("5");
    //   ui->y_set->addItem("7");
    ui->y_set->addItem("10");
    //    ui->y_set->addItem("15");
    //    ui->y_set->addItem("20");
    //    ui->y_set->addItem("50");
    //    ui->y_set->addItem("100");
    //    ui->y_set->addItem("200");
    //    ui->y_set->addItem("500");
    //    ui->y_set->addItem("1000");
    //    ui->y_set->addItem("2000");
    //    ui->y_set->addItem("5000");
    //    ui->y_set->addItem("10000");

    //(7. 数据位
    L = shujuliangint * 3 * 2;
    L2 = L + 12;
    //framedata = "";

    Show_Mes=new MyMessageBox();
    qDebug()<<"初始化完成";

    Imp10k=new MyMessageBox();
    Imp10k->setIcon("init");
    Imp10k->setBodyText("阻抗已经小于10K");
    Imp10k->setButtonText("确定");
    //Show_Mes->exec();
   // Show_Mes->show();


}

MainWindow::~MainWindow()
{
    delete ui;
    delete tcpSocket;
    delete tcpServer;
}

void MainWindow::ALLdata_first_set()
{
    for (int i=0;i<MAX_Slave;i++)
    {
        MAX_Shou[i]= -1.79e30;
        MIN_Shou[i]=  1.79e30;
        time_x[i]= 0;
        show_vale[i]= 0;
    }
    for (int i=0;i<BDF_Save;i++)
    {
        save_sys[i]=0;
    }
    for (int i=0;i<1000;i++)
    {
        buf21[i]=0;
    }

    tcpServer = new QTcpServer();//监听套接字
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::onNewConnect);
    tcpServer->listen(QHostAddress::Any, 1888);

}

void MainWindow::My_UI_show()
{

}


//串口打开or关闭:
void MainWindow::on_serial_clicked()
{

    //多坐标轴绘制
    if(Fir_Star_Con==0)
    {
        for(int i=1;i<chall;i++)
        {
            mPlot->addRect(i);
            qDebug()<<i+1;
        }
        Fir_Star_Con=1;
    }

    //绘图子进程
    mPlot->moveToThread(thread1);
    thread1->start();
   // connect(thread1, SIGNAL(started()), mPlot, SLOT(replot()));
  //  ui->show->moveToThread(thread2);
   // thread2->start();
   // connect(thread2, SIGNAL(started()),  ui->show, SLOT(replot()));

#ifdef SUB_SAVE
    //保存大小判断  20M
    save_size=4500;
    switch(chall)
    {
    case 1:
        save_size=save_size/1;
        break;
    case 2:
        save_size=save_size/2;
        qDebug()<<"保存大小"<<save_size;
        break;
    case 3:
        save_size=save_size/3;
        break;
    case 4:
        save_size=save_size/4;
        break;
    case 5:
        save_size=save_size/5;
        break;
    case 6:
        save_size=save_size/6;
        break;
    case 7:
        save_size=save_size/7;
        break;
    case 8:
        save_size=save_size/8;
        break;
    }
#endif



    //打开串口：
       if(serial_flag == false)
       {
           if (com.isNull() || rate.isNull() || channel.isNull())
           {
                //ui->show_all->setText("提前完成相应设置");
                QMessageBox::information(this,"提示信息","提前完成相应设置");
                serial_flag = false;
                ui->serial->setIconSize(QSize(50,50));
                ui->serial->setIcon(QIcon(":/break off.png"));
           }
           else
           {
               //设置串口号：
               mySer.setPortName(com);

               //设置波特率：
               if(rate == "1200")
               {
                   mySer.setBaudRate(QSerialPort::Baud1200);
               }

               if(rate == "2400")
               {
                   mySer.setBaudRate(QSerialPort::Baud2400);
               }

               if(rate == "4800")
               {
                   mySer.setBaudRate(QSerialPort::Baud4800);
               }

               if(rate == "9600")
               {
                   mySer.setBaudRate(QSerialPort::Baud9600);
               }

               if(rate == "19200")
               {
                   mySer.setBaudRate(QSerialPort::Baud19200);
               }

               if(rate == "38400")
               {
                   mySer.setBaudRate(QSerialPort::Baud38400);
               }

               if(rate == "57600")
               {
                   mySer.setBaudRate(QSerialPort::Baud57600);
               }

               if(rate == "115200")
               {
                   mySer.setBaudRate(QSerialPort::Baud115200);
               }
               if(rate == "3000000")
               {
                   mySer.setBaudRate(rate.toInt());
               }

               bool flag;
               flag  = mySer.open(QSerialPort::ReadWrite);


               //显示设置：
               QString all;
               if(flag)
               {
                   all = "串口号设置为："+ com + "\r" + "波特率设置为：" + rate
                           + "\r" + "通道数为："+ channel + "\r" +"串口打开成功";
                   serial_flag = true;
                   ui->serial->setIconSize(QSize(50,50));
                   ui->serial->setIcon(QIcon(":/connect.png"));
                   ui->rate->setEnabled(false);
                   ui->com->setEnabled(false);
                   ui->channel->setEnabled(false);
                  // ui->lineEdit_2->setEnabled(false); //正式版本需要删除
                   ui->wave->setEnabled(false);
                   this->wavepage->close(); //关闭子窗口
                   data_sample();//设置数据位
                   ui->file->setEnabled(1);
                   ui->save->setEnabled(1);
               }

               else
               {
                   all = "串口打开失败";
                   serial_flag = false;
                   ui->serial->setIconSize(QSize(50,50));
                   ui->serial->setIcon(QIcon(":/break off.png"));
               }
               //ui->show_all->setText(all);
              // QMessageBox::information(this,"提示信息",all);
           }
       }

       //关闭串口：
       else
       {
           ui->show->xAxis->setRange(0, x_range);//修改x的显示范围
           ui->show->yAxis->setRange(-y_range_mm, y_range_mm);//修改y的显示范围
           //-------------------
           for(int i = 0; i<chall; ++i)
           {
               ui->show->graph(i)->data().data()->clear();
               ui->show->graph(i)->addData(0,V_constant.at(i));
           }
           mPlot->replot();
           ui->show->replot();
           for(int i =0; i<all_lab.size();++i)
           {
               //all_line.at(i)->point1->setCoords(-100, 0);
               //all_line.at(i)->point2->setCoords(-100, 1);
               all_lab.at(i)->position->setCoords(-100, 0);
               all_lab.at(i)->setText("");
           }
           m_vline->point1->setCoords(0, 0);
           m_vline->point2->setCoords(0, 1);
           //all_lab.clear();
           //all_line.clear();
           cha_lab= "";
           timerx = 0; //计数测试用
           mPlot->replot();
           ui->show->replot();
           flag_bb = 0;
           framedata_al.clear();
           QVector<QString> ().swap(framedata_al);
           //framedata="";
           cha_lab= "";
           cha_data = "";
           count_num = 0;
           flag_y = true;
           chall_num = 0;
           //-----------------------------------//
           serial_flag = false;
           last_save_flag = false;
           ui->serial->setIconSize(QSize(50,50));
           ui->serial->setIcon(QIcon(":/break off.png"));
           mySer.close();
           last_save_flag =false;
           loss = 0;
           y_rflag = false;
           for(int i = 0; i < chall; ++i)
           {
               ch[i].clear();
           }
           QMessageBox::information(this,"提示信息","串口关闭");
           ui->y_set->setEnabled(true);
           star_flag = false;
           ui->star->setIcon(QIcon(":/star.png"));
           ui->lineEdit->setText("0");
         //  ui->lineEdit_2->setEnabled(true); //正式版本需要删除


           fir_num = 0; //卷积的个数
           fir_flag = false;
           fir_wind.clear();
           for(int i = 0; i < chall; ++i)
           {
               fir_all[i].clear();
           }
           fir_temp.clear();
           fir_start = false;
           ui->wave->setEnabled(true);
           ui->file->setDisabled(1);
           ui->save->setDisabled(1);
       }
}

//串口端口号自动识别:
void MainWindow::on_com_clicked()
{
    ui->com->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
     {
        ui->com->addItem(info.portName());
     }
}

//选择保存路径的文件:
void MainWindow::on_file_clicked()
{
    //----------------------------------------//
    if(serial_flag == true && star_flag == false)
    {
        mySer.close();
        //star_flag = true;
        cha_lab= "";
        timerx = 0; //计数测试用
        ui->show->replot();
        mPlot->replot();
        //-------------------
        flag_bb = 0;
        framedata_al.clear();
        //framedata="";
        cha_lab= "";
        cha_data = "";
        count_num = 0;
        //--------------------
        last_save_flag = false;
        loss = 0;

    }
    //----------------------------------------//

    if(save_path == false)
    {

        QString filex = QFileDialog::getSaveFileName(this,tr("Open File"),"/",tr("Text File(*.txt)"));
        //ui显示：
        QString file_all = "保存的路径为:"+ filex;
        QMessageBox::information(this,"提示信息",file_all);
        File_save = filex;
        //生成txt：
        QFile file(filex);
       // file.open(QFileDevice::Append);
         file.open(QFileDevice::WriteOnly);
        save_path = true;
    }

    else
    {
        QString filex = QFileDialog::getSaveFileName(this,tr("Open File"),File_save,tr("Text File(*.txt)"));
        QString file_all = "保存的路径为:"+ filex;
        //ui->show_all->setText(file_all);
        QMessageBox::information(this,"提示信息",file_all);
        File_save = filex;
        //生成txt：
        QFile file(filex);
        file.open(QFileDevice::WriteOnly);
        save_path = true;
    }

    //----------------------------------------//
    if(serial_flag == true && star_flag == false)
    {
        //star_flag = false;
        mySer.setPortName(com);

        //设置波特率：
        if(rate == "1200")
        {
            mySer.setBaudRate(QSerialPort::Baud1200);
        }

        if(rate == "2400")
        {
            mySer.setBaudRate(QSerialPort::Baud2400);
        }

        if(rate == "4800")
        {
            mySer.setBaudRate(QSerialPort::Baud4800);
        }

        if(rate == "9600")
        {
            mySer.setBaudRate(QSerialPort::Baud9600);
        }

        if(rate == "19200")
        {
            mySer.setBaudRate(QSerialPort::Baud19200);
        }

        if(rate == "38400")
        {
            mySer.setBaudRate(QSerialPort::Baud38400);
        }

        if(rate == "57600")
        {
            mySer.setBaudRate(QSerialPort::Baud57600);
        }

        if(rate == "115200")
        {
            mySer.setBaudRate(QSerialPort::Baud115200);
        }
        if(rate == "3000000")
        {
            mySer.setBaudRate(rate.toInt());
        }

        bool flag;
        flag  = mySer.open(QSerialPort::ReadWrite);
        //-----------打开串口-------------//

        ui->show->xAxis->setRange(0, x_range);//修改x的显示范围
        ui->show->yAxis->setRange(-y_range_mm,y_range_mm);
        for(int i = 0; i<chall; i++)
        {
             ui->show->graph(i)->data().data()->clear();
             ui->show->graph(i)->addData(0,V_constant.at(i));
        }
        mPlot->replot();
        ui->show->replot();
        for(int i =0; i<all_lab.size();++i)
        {
            //all_line.at(i)->point1->setCoords(-100, 0);
            //all_line.at(i)->point2->setCoords(-100, 1);
            all_lab.at(i)->position->setCoords(-100, 0);
            all_lab.at(i)->setText("");
        }
        m_vline->point1->setCoords(0, 0);
        m_vline->point2->setCoords(0, 1);
        //all_lab.clear();
        //all_line.clear();
        timerx = 0; //计数测试用
        mPlot->replot();
        ui->show->replot();
        //------------
        flag_bb = 0;
        framedata_al.clear();
        //framedata="";
        cha_lab= "";
        cha_data = "";
        count_num = 0;
        last_save_flag = false;
        for(int i = 0; i < chall; ++i)
        {
            ch[i].clear();
        }
        flag_y = true;
        chall_num = 0;
    }
     //----------------------------------------//
}


//(1.文件保存与否:
void MainWindow::on_save_clicked()
{
    if(save_flag == false)
    {
        if(File_save.isNull())
        {
            //ui->show_all->setText("请提前设置存储路径");
            QMessageBox::information(this,"提示信息","请提前设置存储路径");
            save_flag = false;
            ui->r_g->setIcon(QIcon(":/red.png"));
        }
        else
        {
            save_flag = true;

#ifndef SUB_SAVE

            pathx.setFileName(File_save);
           // pathx.open(QIODevice::WriteOnly | QFileDevice::Append);
             pathx.open(QIODevice::WriteOnly);
            out.setDevice(&pathx);
            out.setCodec("UTF-8");
#endif
            if(bdf_lab_set == true)
            {
                 set_bdf();
            }



            ui->r_g->setIcon(QIcon(":/green.png"));
        }
    }
    else
    {
        save_flag = false;
        //ui->show_all->setText("停止保存");
        //QMessageBox::information(this,"提示信息","停止保存");
        pathx.close();
        edfclose_file(hdl);
       // bdf_lab_set=false;
        ui->r_g->setIcon(QIcon(":/red.png"));
        Suf=0;
    }
}

//(2.文件保存与否:
void MainWindow::on_r_g_clicked()
{
}

//波特率修改:
void MainWindow::on_rate_currentIndexChanged(const QString &arg1)
{
    rate = arg1;
}

//端口号修改:
void MainWindow::on_com_currentIndexChanged(const QString &arg1)
{
    com = arg1;
}


//通道个数修改:
void MainWindow::on_channel_currentIndexChanged(const QString &arg1)
{

    ui->show->clearGraphs();
    channel = arg1;
    if (channel == "ch1")
    {
           chall = 1;
    }
    if (channel == "ch2")
    {
           chall = 2;
    }
    if (channel == "ch3")
    {
           chall = 3;
    }
    if (channel == "ch4")
    {
           chall = 4;
    }
    if (channel == "ch5")
    {
           chall = 5;
    }
    if (channel == "ch6")
    {
           chall = 6;
    }
    if (channel == "ch7")
    {
           chall = 7;
    }
    if (channel == "ch8")
    {
           chall = 8;
    }
    Create_canvas(chall);
    //dis_test(chall);
}

//显示是否暂停:
void MainWindow::on_star_clicked()
{
    if(serial_flag == true)
    {
        if(star_flag == false)
        {
            mySer.close();
            star_flag = true;
            ui->star->setIcon(QIcon(":/stop.png"));
            cha_lab= "";
            timerx = 0; //计数测试用
            mPlot->replot();
            ui->show->replot();
            //-------------------
            flag_bb = 0;
            framedata_al.clear();
            QVector<QString> ().swap(framedata_al);
            //framedata="";
            cha_lab= "";
            cha_data = "";
            count_num = 0;
            //--------------------
            last_save_flag = false;
            loss = 0;
        }

        else
        {
            star_flag = false;
            ui->star->setIcon(QIcon(":/star.png"));
            mySer.setPortName(com);

            //设置波特率：
            if(rate == "1200")
            {
                mySer.setBaudRate(QSerialPort::Baud1200);
            }

            if(rate == "2400")
            {
                mySer.setBaudRate(QSerialPort::Baud2400);
            }

            if(rate == "4800")
            {
                mySer.setBaudRate(QSerialPort::Baud4800);
            }

            if(rate == "9600")
            {
                mySer.setBaudRate(QSerialPort::Baud9600);
            }

            if(rate == "19200")
            {
                mySer.setBaudRate(QSerialPort::Baud19200);
            }

            if(rate == "38400")
            {
                mySer.setBaudRate(QSerialPort::Baud38400);
            }

            if(rate == "57600")
            {
                mySer.setBaudRate(QSerialPort::Baud57600);
            }

            if(rate == "115200")
            {
                mySer.setBaudRate(QSerialPort::Baud115200);
            }
            if(rate == "3000000")
            {
                 mySer.setBaudRate(rate.toInt()); //设置波特率

            }

            bool flag;
            flag  = mySer.open(QSerialPort::ReadWrite);
            //-----------打开串口-------------//

            ui->show->xAxis->setRange(0, x_range);//修改x的显示范围
            ui->show->yAxis->setRange(-y_range_mm,y_range_mm);
            for(int i = 0; i<chall; i++)
            {
                 ui->show->graph(i)->data().data()->clear();
                 ui->show->graph(i)->addData(0,V_constant.at(i));
            }
            mPlot->replot();
            ui->show->replot();
            for(int i =0; i<all_lab.size();++i)
            {
                //all_line.at(i)->point1->setCoords(-100, 0);
                //all_line.at(i)->point2->setCoords(-100, 1);
                all_lab.at(i)->position->setCoords(-100, 0);
                all_lab.at(i)->setText("");
            }
            m_vline->point1->setCoords(0, 0);
            m_vline->point2->setCoords(0, 1);
            //all_lab.clear();
            //all_line.clear();
            timerx = 0; //计数测试用
            ui->show->replot();
            mPlot->replot();
            //------------
            flag_bb = 0;
            framedata_al.clear();
           // framedata="";
            cha_lab= "";
            cha_data = "";
            count_num = 0;
            last_save_flag = false;
            for(int i = 0; i < chall; ++i)
            {
                ch[i].clear();
            }
            flag_y = true;
            chall_num = 0;
        }
    }

}

//显示窗口初始化:
void MainWindow:: displot_intial()
{


    //画布-示波器优化：

    QFont font = QFont(AppFontName, AppFontSize - 2);
    ui->show->xAxis->setLabelFont(font);
    ui->show->yAxis->setLabelFont(font);
    ui->show->xAxis->setTickLabelFont(font);
    ui->show->yAxis->setTickLabelFont(font);
    ui->show->yAxis->setLabelColor(TextColor);
    ui->show->yAxis->setTickLabelColor(TextColor);
    ui->show->yAxis->setBasePen(QPen(TextColor, TextWidth));
    ui->show->yAxis->setTickPen(QPen(TextColor, TextWidth));
    ui->show->yAxis->setSubTickPen(QPen(TextColor, TextWidth));
    //设置坐标颜色/坐标名称颜色
    ui->show->xAxis->setTickLabelColor(TextColor);

    ui->show->xAxis->setBasePen(QPen(TextColor, TextWidth));

    ui->show->xAxis->setTickPen(QPen(TextColor, TextWidth));

    ui->show->xAxis->setSubTickPen(QPen(TextColor, TextWidth));

    //设置画布背景色
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->show->setBackground(plotGradient);
    mPlot->setBackground(plotGradient);

    //设置坐标背景色
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    ui->show->axisRect()->setBackground(axisRectGradient);
    mPlot->setBackground(plotGradient);


    //设置图例提示位置及背景色
    //ui->show->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
    //ui->show->legend->setBrush(QColor(255, 255, 255, 200));
    //ui->show->replot();
}


//恢复默认显示范围:
//显示功能切换
void MainWindow::on_initialization_clicked()
{


    if(Now_Function==Single_Show)
    {
        Now_Function=Mult_Show;
        ui->stackedWidget->setCurrentIndex(1);
       // connect(thread1, SIGNAL(started()), mPlot, SLOT(replot()));
      //  disconnect(thread2, SIGNAL(started()),  ui->show, SLOT(replot()));
    }

    else
    {
        Now_Function=Single_Show;
        ui->stackedWidget->setCurrentIndex(0);
      //  disconnect(thread1, SIGNAL(started()), mPlot, SLOT(replot()));
      //  connect(thread2, SIGNAL(started()),  ui->show, SLOT(replot()));


    }
    qDebug()<<"当前功能"<<Now_Function;
    ui->show->replot();
    mPlot->replot();
//    ui->show->xAxis->setRange(0, x_range);//修改x的显示范围
//    ui->show->yAxis->setRange(-y_range_mm, y_range_mm);//修改y的显示范围
//    ui->show->yAxis->setTickLabels(false); //删除y轴
//    ui->show->yAxis->grid()->setVisible(false);//不显示y轴网格




//    for(int i=0;i<chall;i++)
//    {
//        mPlot->RectList[i]->axis(QCPAxis::atBottom)->setRange(0, x_range);
//        mPlot->RectList[i]->axis(QCPAxis::atLeft)->setRange(-y_range_mm, y_range_mm);
//        mPlot->RectList[i]->axis(QCPAxis::atLeft)->setTickLabels(true); //删除y轴
//        mPlot->RectList[i]->axis(QCPAxis::atLeft)->grid()->setVisible(true);//不显示y轴网格
//    }

}

//减少x轴显示范围:
void MainWindow::on_x_down_clicked()
{
    x_range = x_range - 1;
    if(x_range<=1)
    {
        x_range = 1;
    }
    ui->show->xAxis->setRange(0,x_range);//修改x的显示范围

    for(int i=0;i<chall;i++)
    {
        mPlot->RectList[i]->axis(QCPAxis::atBottom)->setRange(0, x_range);
    }



    mPlot->replot();
    ui->show->replot();
}

//增加x轴显示范围:
void MainWindow::on_x_up_clicked()
{
    x_range = x_range + 1;
    if(x_range>=20)
    {
        x_range = 20;
    }
    ui->show->xAxis->setRange(0, x_range);//修改x的显示范围


    for(int i=0;i<chall;i++)
    {
        mPlot->RectList[i]->axis(QCPAxis::atBottom)->setRange(0, x_range);
    }



    mPlot->replot();
    ui->show->replot();
}

//创建画布:
void MainWindow:: Create_canvas(double num)
{
    V_constant.clear();
    double flag = (y_range2_mm/num)/2;
    for(int i = 0; i < num; ++i)
    {
        double k = (flag+i*(y_range2_mm/num));
        k = k - y_range2_mm/2;
        ui->show->addGraph(); //初始化绘图控件
        ui->show->graph(i)->setPen(colour.at(i));
        ui->show->graph(i)->setAntialiasedFill(true);
        ui->show->graph(i)->addData(0,k);
        ui->show->legend->setVisible(true);
        ui->show->legend->setFont(QFont("Helvetica", 15));
        ui->show->legend->setBrush(QColor(255,255,255,0));//设置图例背景
       // ui->show->legend->setBorderPen(Qt::NoPen);//设置边框隐藏


        V_constant.append(k);
    }
    ui->show->replot();
    mPlot->replot();
}

//通道测试程序，正式版本需要删除:
void MainWindow:: dis_test(double num)
{
    if(all_linex.size()==0)
    {
        for(int i =0; i<8; ++i)
        {
            QCPItemStraightLine *m_vline = new QCPItemStraightLine(ui->show);
            m_vline->setLayer("overlay");
            // 颜色随机
            m_vline->setPen(QPen(Qt::blue));
            // 超出坐标轴范围则不显示游标线
            m_vline->setClipToAxisRect(true);
            // 画竖线，x为curtime，y只要0和1即可绘制直线了
            m_vline->point1->setCoords(0, -1000);
            m_vline->point2->setCoords(1, -1000);
            all_linex.append(m_vline);
        }
    }
    else
    {
       for(int i =0; i<8; ++i)
       {
           all_linex.at(i)->point1->setCoords(0,-1000);
           all_linex.at(i)->point2->setCoords(1,-1000);
       }
    }
    double flag = (y_range2_mm/num)/2;
    for(int i = 0; i < num; ++i)
    {
        double k = (flag+i*(y_range2_mm/num));
        k = k - y_range2_mm/2;
        all_linex.at(i)->point1->setCoords(0, k);
        all_linex.at(i)->point2->setCoords(1, k);
    }
    ui->show->replot();
    mPlot->replot();
}



//y轴显示范围测试:
void MainWindow::on_y_set_currentIndexChanged(const QString &arg1)
{

//        //y_rflag = true;
//        flag_y = false;
//        for(int i = 0; i<chall; ++i)
//        {
//            ui->show->graph(i)->data().data()->clear();
//            ui->show->graph(i)->addData(0,V_constant.at(i));
//        }

//        variable_y = 50/arg1.toDouble();
//        for(int j = 0; j<chall; ++j)
//        {
//            for(int i = 0; i < ch[j].length(); i++)
//            {
//                double l = variable_y * ch[j].at(i)  + V_constant.at(j);
//                ui->show->graph(j)->addData(i+1, l);
//            }
//            ui->show->replot();
//        }
//        flag_y = true;


}

//从机初始化判断
void MainWindow::initAnay(QString str)
{
    if(str.startsWith("Initiate service discover"))
    {
        if(init_pd_flag!=0)
        {
            return;
        }
        disconn_slaver.clear();
        disconn_pd_flag=0;
        disconn_slaver.clear();
       // disconn_slaver="失去连接从机ID为：";
        QString all;
        all = "串口号为："+ com + "\r" + "波特率为：" + rate
                + "\r" + "通道数为："+ channel ;
        init_pd_flag=1;
        Show_Mes->setIcon("init");
        Show_Mes->setBodyText(all);
        Show_Mes->setButtonText("从机初始化请稍后");
        //Show_Mes->exec();
        Show_Mes->show();
    }
}
//连接判断判断
void MainWindow::connAnay(QString str)
{
    if(str.startsWith("EEG_SM_TIME_RESP"))
    {
        if(conn_pd_flag!=0)
        {

            return;
        }
        disconn_slaver.clear();
       // disconn_slaver="失去连接从机ID为：";
        disconn_pd_flag=0;
        QString all;
        all = "串口号为："+ com + "\r" + "波特率为：" + rate
                + "\r" + "通道数为："+ channel ;
        init_pd_flag=1;
        conn_pd_flag=1;
        Show_Mes->setIcon("init");
        Show_Mes->setBodyText(all);
        Show_Mes->setButtonText("EEG_SM_TIME_RESP");
        //Show_Mes->exec();
        Show_Mes->show();
    }
}
//从机断开连接判断
void MainWindow::disconnAnay(QString str)
{
    if(str.startsWith("GAP_LINK_TERMINATED_EVENT") || disconn_pd_flag==1 )
    {
        disconn_pd_flag=1;
        Dis_Situation=Dis_Situation+str;
        DisConAnay(Dis_Situation);
    }

}

void MainWindow::DisConAnay(QString str)
{
  //   qDebug()<<str;
    QString Find_key=" is disconnectedNum Conns";
    int Local_Loc;
    if(str.contains(Find_key,Qt::CaseSensitive)==true)
    {
        Local_Loc=str.indexOf(Find_key);
        disconn_slaver=disconn_slaver+"0X"+str.mid(Local_Loc-2,2)+"号从机"+"  ";
        Show_Mes->setIcon("error");
        Show_Mes->setBodyText("出现错误：失去连接从机ID为："+disconn_slaver);
        Show_Mes->setButtonText("请重启系统");
        Show_Mes->show();
        Dis_Situation.remove(1,Local_Loc+28);

    }

}

//Y轴定标值计算    每毫米的计算 微V计算
void MainWindow::Calibration_value(int Chall_show_value,double unint_mv)
{
    double value;
    QString Calibration_value_show;
    if(Now_Function==Single_Show)
    {
        QCPAxis *yAxis=ui->show->axisRect()->axis(QCPAxis::atLeft);
        QScreen *screen = QGuiApplication::primaryScreen();
        qreal dpi = screen->logicalDotsPerInch(); // 获取屏幕 DPI
        double yAxisPixelLength = yAxis->axisRect()->height();
        qreal cmLength_y = yAxisPixelLength / dpi * 2.54; // 将像素长度转换为厘米
        value=unint_mv/cmLength_y;
        int Value=value;
        ui->lineEdit_2->setText(QStringLiteral("厘米-毫伏 ")+QString::number(Value/1000.0));

    }
    else
    {
        QCPAxis *yAxis = mPlot->RectList[Chall_show_value]->axis(QCPAxis::atLeft);
        QScreen *screen = QGuiApplication::primaryScreen();
        qreal dpi = screen->logicalDotsPerInch(); // 获取屏幕 DPI
        double yAxisPixelLength = yAxis->axisRect()->height();
        qreal cmLength_y = yAxisPixelLength / dpi * 2.54; // 将像素长度转换为厘米
        value=unint_mv/cmLength_y;
        show_vale[Chall_show_value]=value;

        if(Chall_show_value+1==chall)
        {
            Calibration_value_show.clear();
            for (int i=0;i<chall;i++) {
                Calibration_value_show=Calibration_value_show+ QString::number(i,10)+QStringLiteral("窗")+QString::number(show_vale[i]/1000.0)+QStringLiteral(" ");
            }
            QString Calibration_show =QStringLiteral("厘米-毫伏 ")+Calibration_value_show;
            ui->lineEdit_2->setText(Calibration_show);
            ui->lineEdit_2->setFixedWidth( ui->lineEdit_2->fontMetrics().width(ui->lineEdit_2->text()) + 10);
        }
    }
}


//串口接受线程：
void MainWindow::displayData()
{
    //*串口接受方式2*//
    QByteArray rx_data;
    QString framedata;

    rx_data = this->mySer.readAll(); //串口数据接受

//    //  qDebug()<<rx_data;
//    //命令分析
    initAnay(rx_data);
    connAnay(rx_data);
    disconnAnay(rx_data);

    //接收数据分析
    for(int i = 0; i<rx_data.length(); ++i)
    {
        framedata_al.append(rx_data.mid(i,1).toHex().trimmed().toUpper());

    }

    //寻找数据头FF/ff，如果找到了数据头，删除数据头前所有数据
    if(flag_bb == 0)
        {

            for(int i = 0 ; i<framedata_al.length(); ++i)
            {
                if(framedata_al.at(i) == "FF" || framedata_al.at(i) == "ff")
                {
                    if(i != 0)
                    {
                        framedata_al.remove(0,i);
                      //  framedata_al.shrink_to_fit();
                      //  QVector<QString> ().swap(framedata_al);
                    }
                    else if(i == 0)
                    {
                       //  qDebug()<<all2;


                        if(framedata_al.length()>=all2)
                        {
                           // if(framedata_al[i].mid((i+6*chall)*2,2)=="AA")

                            for(int i = 0; i<all2; ++i)
                            {
                                framedata = framedata + framedata_al.at(i);
                            }
                            framedata_al.remove(0,all2);

                            chuli_muli_new_fir(framedata,L,L2);//多通道新格式+fir
                             framedata = "";

                        }
                    }
                    flag_bb = 1;
                    break;
                }
            }
        }
        else
        {
            if(conn_pd_flag==1)
            {
                 Show_Mes->close();
                 conn_pd_flag=2;
            }

           if(framedata_al.length()>=all2)
           {
               for(int i = 0; i<all2; ++i)
               {
                  framedata = framedata + framedata_al.at(i);
               }
               framedata_al.remove(0,all2);
             //  framedata_al.squeeze();
               // QVector<QString> ().swap(framedata_al);
               chuli_muli_new_fir(framedata,L,L2);//多通道新格式+fir
              // framedata = "";
              // framedata.clear();

           }
        }
}


//画标签和游标2：
void MainWindow::createVLineTag2(QString val, int x, double y)
{

    // 设置游标垂直线
    //QCPItemStraightLine *m_vline = new QCPItemStraightLine(ui->show);
    //m_vline->setLayer("overlay");
    // 颜色随机
    //m_vline->setPen(QPen(Qt::red));
    // 超出坐标轴范围则不显示游标线
    //m_vline->setClipToAxisRect(true);
    // 画竖线，x为curtime，y只要0和1即可绘制直线了
    //m_vline->point1->setCoords(x, 0);
    //m_vline->point2->setCoords(x, 1);
    //all_line.append(m_vline);
    // 设置文本框
    // 超出坐标轴范围则不显示标签
    QCPItemText *m_currentLabel = new QCPItemText(ui->show);
    m_currentLabel->setClipToAxisRect(true);
    m_currentLabel->setPadding(QMargins(3, 3, 3, 3));
    m_currentLabel->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    m_currentLabel->setBrush(Qt::NoBrush);
    // 可以设置显示位置跟随锚点的位置，此次我设置的是绝对位置，添加TAG用的
    m_currentLabel->position->setCoords(x, y);
    m_currentLabel->setFont(QFont("Arial", 8));
    m_currentLabel->setColor(Qt::red);
    m_currentLabel->setText(val);
    all_lab.append(m_currentLabel);
}

//画标签和游标3：
void MainWindow::createVLineTag3(QString val, int x, double y, int num)
{
    //all_line.at(num)->point1->setCoords(x, 0);
    //all_line.at(num)->point2->setCoords(x, 1);
    all_lab.at(num)->position->setCoords(x, y);
    all_lab.at(num)->setText(val);
}


//显示波形设置界面:
void MainWindow::on_wave_clicked()
{
     this->wavepage->show();
}

//滤波函数--多通道:
void MainWindow::filtering_muli()
{
    fir_flag = true;
}

//恢复至原始波形:
void MainWindow::recovery_muli()
{
    qDebug()<<ch[0];
    qDebug()<<ch[0].length();
}

//test----输入数据量正式版本需要删除:
void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{
//    shujuliangint = ui->lineEdit_2->text().toInt();
//    QString k = QString::number(shujuliangint);
//    ui->lineEdit_2->setText(k);
}


//new-------多通道新格式:
//1).多通道数据格式设置:
void MainWindow::data_sample()
{
    qDebug()<<shujuliangint;
    L = shujuliangint*3*2;//多通道所有数据量
    all2 = (L*chall + chall*8 + 12)/2;//8为数据的报头和报尾, 12为AA~BB
    L2 = L + 12;
    all3 = (shujuliangint * 3 * 2)+4;
    fir_num = shujuliangint*2 + 1;

    //滤波参数设置
    if(fir_flag == false)
    {
        for(auto i = 0; i<fir_num; ++i)
        {
            fir_wind.append(1);
        }
    }
    else
    {
        fir_cez* fir = new fir_cez;
        if(this->wavepage->filtering_flag == 1) //low
        {
            fir->design(this->wavepage->low_f,fir_num,this->wavepage->filtering_flag);
        }

        if(this->wavepage->filtering_flag == 2) //high
        {
             fir->design(this->wavepage->high_f,fir_num,this->wavepage->filtering_flag);
        }

        if(this->wavepage->filtering_flag == 3) //notch
        {
            //fir->design(this->wavepage->notch_f,fir_num,this->wavepage->filtering_flag);
            fir_xianbo(1000,this->wavepage->notch_f);
        }

        fir_wind = fir->coefs;
    }

}

bool ok;
//标签判断
//now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
int MainWindow::LabAnary(QString str)
{
   // qDebug()<<str; 25  00  00  00  BB
    if(str.mid(2,2)=="00")
    {
        return 0 ;
    }
    else
    {
        //标签的位置
        int label_local =str.mid(6,2).toInt(&ok,16);
        //标签的内容 4分类1，2，3，4
        int label_txt =str.mid(2,2).toInt(&ok,16);;
        qDebug()<<"标签解析的是所有标签，包括每次实验时，可能会多出的那个";
        qDebug()<<"First标签解析"<<label_local<<"   "<<label_txt;


        now_label_txt[0]=label_txt;
        now_label_txt[1]=label_local;
        MetalStart++;
        return 1;
    }
}

//2.1).new多通道处理+fir滤波
void MainWindow::chuli_muli_new_fir(QString rec, int L, int L2)
{
   //   qDebug()<<rec;
//      qDebug()<< shujuliangint*chall + chall*8;
//      qDebug()<< rec.mid((shujuliangint*chall*6 + chall*8),2);
    //判断数据包是否有所用从机数据
    if (rec.mid((shujuliangint*6*chall + chall*8),2)!="AA" && rec.mid((shujuliangint*6*chall + chall*8)-2,2)!="FE")
    {
        return;
    }



    else
    {
        LabAnary(rec.mid(shujuliangint*6*chall + chall*8 + 2,10));
        //now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
        //在线数据发送标志
        if(online == 1&& onlien_startsend ==1)
        {
            Onlie_Use_Dada(rec);
        }


        //            qDebug()<<rec;
        //            qDebug()<<rec.mid(shujuliangint*6*chall + chall*8 + 2,10);
        if(save_flag == true && flag_y == true)
        {
#ifdef SUB_SAVE
            //分包存储
            if(Suf >= save_size)
            {
                Suf=1;
                File_save_suf++;
                File_save_change=File_save.left(File_save.length() - 4)+QStringLiteral("_")+QString::number(File_save_suf,10)+QStringLiteral(".txt");
            //    qDebug()<<File_save;
            }
            if(Suf == 0)
            {
                QFile file_2(File_save);
                if (!file_2.open(QFile::ReadWrite)) {
                }
                // 关闭文件并删除
                file_2.close();
                if (!file_2.remove()) {

                }

                File_save_change=File_save.left(File_save.length() - 4)+QStringLiteral("_")+QString::number(File_save_suf,10)+QStringLiteral(".txt");
            //    qDebug()<<File_save_change;
            }

            //文件流写入
            //QFile pathx(File_save_change);
            pathx.setFileName(File_save_change);
            pathx.open(QIODevice::WriteOnly | QFileDevice::Append);
            out.setDevice(&pathx);
            out.setCodec("UTF-8");
#endif
            QString rec_all;
            //添加空格
            for(int i = 0; i < rec.length()/2; ++i)
            {
                int k = i*2;
                rec_all = rec_all + rec.at(k);
                rec_all = rec_all + rec.at(k+1);
                rec_all = rec_all + QStringLiteral(" ");
            }
            QByteArray res = rec_all.toUtf8();
            out <<res;
//            大量文件写入测试//  out <<res;out <<res;out <<res;out <<res;out <<res;out <<res;

            out.flush();
            // pathx.write(res);

        }
        //曲线名称图例设置  bdf标签设置
        for(int legset=0;legset<chall;legset++)
        {
            ui->show->graph(legset)->setName(rec.mid(legset*(shujuliangint*3+4)*2+2,2));//设置名称
            mPlot->setLegen(legset,0,rec.mid(legset*(shujuliangint*3+4)*2+2,2));
            if(bdf_lab_flag==false)
            {
                bdf_lab.append(rec.mid(legset*(shujuliangint*3+4)*2+2,2));
                if(legset==chall-1)
                {
                    bdf_lab_flag=true;

                }
            }

        }
        //标签ID自动添加，获取保存文件名+通道ID
        if(save_path==true &&  bdf_lab_flag==true && bdf_lab_set==false )
        {
             //set_bdf();//打开一个bdf文件并设置
             bdf_lab_set=true;
        }

        //(3.数据处理:
        if(flag_y == true)
        {
            for(auto i_num = 0; i_num<chall; ++i_num)
            {
                int now_channel=(i_num+1)%chall;
                //检测丢包:3.4屏蔽功能
                //data_verification_all(rec.mid(2,2),rec.mid(all3,2));

                cha_data = rec.mid(4,L);
                //qDebug()<<cha_data;
                //不使用fir滤波:
                if(fir_flag == false)
                {
//                    qDebug()<<"cha_data.length()/6"<<cha_data.length()/6;
                    for(int i = 0; i<(cha_data.length()/6); ++i)
                    {
                        //timerx之后修改为与采样率
                        time_x[now_channel]= time_x[now_channel]+0.001;
#ifdef SUB_SAVE
                        if(save_flag == true)
                        {
                            Suf=Suf+0.001;
                        }
#endif
                        if (time_x[now_channel]>x_range)
                        {
                            time_x[now_channel]=0;
                            //单坐标清除
                            ui->show->graph(chall_num)->data().data()->clear();
                            //多坐标清除
                            for(int i=0;i<chall;i++)
                            {
                                mPlot->clearData(i,0);
                                MAX_Shou[i]=-1.79e308;
                                MIN_Shou[i]=1.79e308;
                            }

                             Max=-1.79e308;
                             Min=1.79e308;

                        }
                       // timerx = timerx + 0.001;
                        int k = i*6;
                        QString str = cha_data.mid(k,6);
                        int ix = str.toInt(&OK,16); //转16进制整形
                        double b = ix; //转double
                        // double eeg = ((b-8388608)*0.1442432/12);
                         double eeg;
                         if(EEG_IMP_Function==IMP_CONN)
                         {
                             eeg = ((b-8388608)*0.480772);
                         }
                         if(EEG_IMP_Function==EEG_CONN)
                         {
                             eeg = ((b-8388608)*0.480772/12);
                         }

                       // qDebug()<<eeg;
                        if (eeg>Max)
                        {
                            Max=eeg;

                        }
                        if (eeg<Min)
                        {
                            Min=eeg;
                        }

                        if (eeg>MAX_Shou[i_num])
                        {
                            MAX_Shou[i_num]=eeg;
                        }
                        if (eeg<MIN_Shou[i_num])
                        {
                            MIN_Shou[i_num]=eeg;
                        }
                        //eeg数据存储进bdf文件
                        //now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
                        if(save_flag == true)
                        {
                            if(now_label_txt[1]!=0)
                            {
                                eventsave =1;
                            }

                           BDF_data(i_num,eeg);
                        }

                       // ch[chall_num].append(eeg);
                        //y轴设置
                       // ui->show->yAxis->setRange(Min-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+Max);//修改y的显示范围
                       // mPlot->RectList[i_num]->axis(QCPAxis::atLeft)->setRange(MIN_Shou[i_num]-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+ MAX_Shou[i_num]);
                        if(Now_Function==Mult_Show)
                        {
                            if(EEG_IMP_Function==IMP_CONN)
                            {


                                double NOW_Implace = (eeg/2*16777216)/4.033/6/1000000;
                                NOW_Implace =  (NOW_Implace-851661.844)/1000;

                                if(NOW_Implace>0.5)
                                {
                                    if(LAST_Implace-NOW_Implace<2 && NOW_Implace-LAST_Implace<2)
                                    {}
                                    else
                                    {
//                                        ui->show->yAxis->setRange(NOW_Implace-2, NOW_Implace+2);//修改y的显示范围
//                                        mPlot->RectList[i_num]->axis(QCPAxis::atLeft)->setRange(NOW_Implace-2, NOW_Implace+2);
                                    }

//                                    qDebug()<<NOW_Implace;
//                                    qDebug()<<"last"<<LAST_Implace;
                                      LAST_Implace = NOW_Implace;

//                                      mPlot->addData(i_num,0,time_x[now_channel],NOW_Implace);
                                      int ColIndex =int(NOW_Implace/20000);
                                      if (ColIndex>=5)
                                         {
                                              ColIndex=5;
                                         }
                                      QString col = QString("QLabel{background-color:%1;}").arg(Allcol[ColIndex]);
                                      ui->SlaveIR_1->setStyleSheet(col);




                                      if(IMPPD == 1&& NOW_Implace>10)
                                     {
//                                       Imp10k->hide();
//                                       IMPPD = 0;
                                       }
                                }
                               if(NOW_Implace>-100)
                                {
                                    if(IMPPD == 0 )
                                    {
                                          Imp10k->show();
                                          IMPPD=1;
                                    }
                                }

                            }
                            if(EEG_IMP_Function==EEG_CONN)
                            {
                                ui->show->yAxis->setRange(Min-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+Max);//修改y的显示范围
                                mPlot->RectList[i_num]->axis(QCPAxis::atLeft)->setRange(MIN_Shou[i_num]-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+ MAX_Shou[i_num]);
                                mPlot->addData(i_num,0,time_x[now_channel],eeg);
                                Calibration_value(i_num,(ui->y_set->currentText().toInt()*1000+ Max)-(Min-(ui->y_set->currentText().toInt()*1000)));
                            }


                           // mPlot->addData(i_num,0,time_x[now_channel],eeg);
                           // Calibration_value(i_num,(ui->y_set->currentText().toInt()*1000+ MAX_Shou[i_num])-(MIN_Shou[i_num]-(ui->y_set->currentText().toInt()*1000)));
                        }
                        else {
                            if(EEG_IMP_Function==IMP_CONN)
                            {
                                ui->stackedWidget->setCurrentIndex(1);
                                double NOW_Implace = (eeg/2*16777216)/4.033/6/1000000;
                                NOW_Implace =  (NOW_Implace-851661.844)/1000;

                                if(NOW_Implace>0.5)
                                {

                                    if(LAST_Implace-NOW_Implace<2 && NOW_Implace-LAST_Implace<2)
                                    {}
                                    else
                                    {
//                                        ui->show->yAxis->setRange(NOW_Implace-2, NOW_Implace+2);//修改y的显示范围
//                                        mPlot->RectList[i_num]->axis(QCPAxis::atLeft)->setRange(NOW_Implace-2, NOW_Implace+2);
                                    }


                                    LAST_Implace = NOW_Implace;
//                                    ui->show->graph(chall_num)->addData( time_x[now_channel], NOW_Implace);
                                    int ColIndex =int(NOW_Implace/10);
                                    if (ColIndex>=5)
                                    {
                                        ColIndex=5;
                                    }
                                    qDebug()<<NOW_Implace;
                                    qDebug()<<ColIndex;
                                    QString col = QString("QLabel{background-color:%1;}").arg(Allcol[ColIndex]);
                                    ui->SlaveIR_1->setStyleSheet(col);

                                    if(IMPPD == 1 && NOW_Implace>10 )
                                    {
//                                        Imp10k->hide();
//                                        IMPPD = 0;
                                    }

                                }
//                                else
//                                {
//                                    QString col = QString("QLabel{background-color:%1;}").arg(Allcol[0]);
//                                }

//                               if(NOW_Implace>-40)
//                                {
//                                   qDebug()<<"pd";

//                                    if(IMPPD == 0)
//                                    {
//                                          Imp10k->show();
//                                          qDebug()<<"show";
//                                          IMPPD=1;
//                                    }

//                                }



                            }
                            if(EEG_IMP_Function==EEG_CONN)
                            {
                                ui->show->yAxis->setRange(Min-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+Max);//修改y的显示范围
                                mPlot->RectList[i_num]->axis(QCPAxis::atLeft)->setRange(MIN_Shou[i_num]-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+ MAX_Shou[i_num]);
                                ui->show->graph(chall_num)->addData( time_x[now_channel], eeg);
                                Calibration_value(i_num,(ui->y_set->currentText().toInt()*1000+ Max)-(Min-(ui->y_set->currentText().toInt()*1000)));
                            }


                      }

                    }
                }
                //使用fir滤波:
                else
                {
                    for(int i = 0; i<(cha_data.length()/6); ++i)
                    {
                        int k = i*6;
                        QString str = cha_data.mid(k,6);
                        int ix = str.toInt(&OK,16); //转16进制整形
                        double b = ix; //转double
                        double eeg = ((b-8388608)*0.480772/12);
                        fir_all[chall_num].append(eeg);
                    }
                    if(this->wavepage->filtering_flag == 1 || this->wavepage->filtering_flag == 2)
                    {
                        //fir 数据头
                        if(fir_start == false)
                        {
        //                    if(fir_all[chall_num].length()==(fir_num-1)/2)
        //                     {
        //                        for(auto i = 0; i<(fir_num-1)/2; ++i)
        //                        {
        //                            timerx = timerx + 1;
        //                            double eeg2 = fir_all[chall_num][i];
        //                            ch[chall_num].append(eeg2);
        //                            eeg2 = variable_y * (eeg2) + V_constant.at(chall_num);
        //                            ui->show->graph(chall_num)->addData(timerx, eeg2);
        //                        }
        //                     }
                        }

                        //fir卷积
                        else
                        {
                            while(fir_all[chall_num].length()>fir_num)
                            {
                                double eeg2 = 0;
                                timerx = timerx + 1;
                                for(auto i = 0; i<fir_num; ++i)
                                {
                                    eeg2 = eeg2 + fir_all[chall_num][i]*fir_wind[i];
                                }
                                ch[chall_num].append(eeg2);
                                eeg2 = variable_y * (eeg2) + V_constant.at(chall_num); //eeg处理
                                ui->show->graph(chall_num)->addData(timerx, eeg2);
                                fir_all[chall_num].remove(0,1);
                            }
                             //fir数据尾
        //                    if(fir_all[chall_num].length()<=(fir_num-1)/2)
        //                     {

        //                        for(auto i = 0; i<fir_all[chall_num].length(); ++i)
        //                        {
        //                            timerx = timerx + 1;
        //                            double eeg2 = fir_all[chall_num][i];
        //                            ch[chall_num].append(eeg2);
        //                            eeg2 = variable_y * (eeg2) + V_constant.at(chall_num);
        //                            ui->show->graph(chall_num)->addData(timerx, eeg2);
        //                        }

        //                     }

                        }
                    }
                    if(this->wavepage->filtering_flag == 3)
                    {
                        while(fir_all[chall_num].length() > 3)
                        {
                            double eeg2 = 0;
                            timerx = timerx + 1;
                            for(auto i = 0; i<3; ++i)
                            {
                                eeg2 = eeg2 + fir_all[chall_num][i]*xian[i];
                            }
                            eeg2  = eeg2/3;
                            ch[chall_num].append(eeg2);
                            eeg2 = variable_y * (eeg2) + V_constant.at(chall_num); //eeg处理
                            ui->show->graph(chall_num)->addData(timerx, eeg2);
                            fir_all[chall_num].remove(0,1);
                        }
                    }

                }

                 rec.remove(0,(L+8));
                 //qDebug()<<now_channel;
                 chall_num = chall_num + 1;
                 if(chall_num == chall)
                 {
                     fir_start = true;//加载完所有数据
                     chall_num = 0;
                     m_vline->point1->setCoords(timerx, 0);
                     m_vline->point2->setCoords(timerx, 1);
                     ui->show->replot();
                     // mPlot->qreplot();
                     // mPlot->replot(QCustomPlot::rpQueued);
                 }
                 else
                 {
                    // timerx = timerx - cha_data.length()/6;
                 }
            }
        }

        //(4.标签处理:
//        if(chall == 1)
//        {
//            cha_lab = rec.mid(4,10); //标签
//            QString str_lab1 = cha_lab.mid(0,2); //标签类型
//            QString str_lab2 = cha_lab.mid(2,2); //标签次数
//            int lab1 = str_lab1.toInt(&OK,16);
//            int lab2 = str_lab2.toInt(&OK,16);
//            if(lab1 != 0 && lab2 != 0)
//            {
//                QString str_lab_set = cha_lab.mid(4,2); //标签位置
//                int lab = str_lab_set.toInt(&OK,16); //转16进制
//                int k_lab =  timerx + lab;
//                QString showxx =  str_lab1 + "--" + str_lab2;
//                if(first_flag == 0)
//                {
//                  createVLineTag2(showxx, k_lab, -y_range_mm+16);
//                }
//                else
//                {
//                    if(count_num <= (all_lab.length()-1))
//                    {
//                        createVLineTag3(showxx, k_lab, -y_range_mm+16, count_num);
//                        count_num = count_num + 1;
//                    }
//                    else
//                    {
//                       createVLineTag2(showxx, k_lab, -y_range_mm+16);
//                    }
//                }
//                ui->show->replot();
//                mPlot->replot();
//            }
//        }

        //(5.清除:
//        if (timerx >= x_range)
//        {

//            first_flag = 1;
//            for(int i =0; i<all_lab.size();++i)
//            {
//                //all_line.at(i)->point1->setCoords(-100, 0);
//                //all_line.at(i)->point2->setCoords(-100, 1);
//                all_lab.at(i)->position->setCoords(-100, 0);
//                all_lab.at(i)->setText("");
//            }
//            count_num = 0;
//            timerx = 0;
//            for(int i =0; i < chall; ++i)
//            {
//                ch[i].clear();

//                ui->show->graph(i)->data().data()->clear();
//                ui->show->graph(i)->addData(0,V_constant.at(i));
//            }
//           mPlot->replot();
//            ui->show->replot();

//        }
//    }


    }
}

//3).new数据校验
void MainWindow::data_verification_new(int *now,int* last)
{
    int now_temp=*now;
    int last_temp=*last;

    // now_save是&rec
    if(last_save_flag <chall)
    {
        *last = now_temp; //上一次
        last_save_flag++;
        loss = 0;
    }
    else
    {
        //情况1：
        if(last_temp == now_temp)
        {
            loss = loss + 1;
        }

        //情况2：
        else if (now_temp - last_temp > 1)
        {
            loss = loss + 1;
        }

        //情况3：
        else if (now_temp - last_temp <1 && now_temp != 0 && last_temp != 255)
        {
            loss = loss + 1;
        }
       QString loss_s = QString::number(loss,10);
       ui->lineEdit->setText(loss_s);
        *last = now_temp; //上一次
    }

}


//3).多通道丢包检测
void MainWindow::data_verification_all(QString head,QString rec)
{
     int now_channel = head.toInt(&OK,16); //选择通道
    // qDebug()<<"多通道丢包检测当前通道"<<now_channel;
     switch(now_channel)
     {
          case 0x01:

          now_save[0]=rec.toInt(&OK,16);
           data_verification_new(&now_save[0],&last_save[0]);
    //  qDebug()<<"0x01通道";
          break;

          case 0x02:
          now_save[1]=rec.toInt(&OK,16);
          data_verification_new(&now_save[1],&last_save[1]);
    //  qDebug()<<"0x02通道";
          break;

          case 0x03:
          now_save[2]=rec.toInt(&OK,16);
          data_verification_new(&now_save[2],&last_save[2]);
    //  qDebug()<<"0x03通道";
           break;

           case 0x04:
           now_save[3]=rec.toInt(&OK,16);
           data_verification_new(&now_save[3],&last_save[3]);
    //  qDebug()<<"0x04通道";
           break;

           case 0x05:
           now_save[4]=rec.toInt(&OK,16);
           data_verification_new(&now_save[4],&last_save[4]);
    //  qDebug()<<"0x05通道";
            break;

            case 0x06:
            now_save[5]=rec.toInt(&OK,16);
            data_verification_new(&now_save[5],&last_save[5]);
       //  qDebug()<<"0x06通道";
             break;

             case 0x07:
             now_save[6]=rec.toInt(&OK,16);
             data_verification_new(&now_save[6],&last_save[6]);
      //   qDebug()<<"0x07通道";
             break;

            case 0x08:
            now_save[7]=rec.toInt(&OK,16);
            data_verification_new(&now_save[7],&last_save[7]);
   //   qDebug()<<"0x08通道";
            break;
     }


}

//4).零点法陷波器:
void MainWindow::fir_xianbo(double fs, double f)
{
    xian.clear();
    double wo = TWOPI*(f/fs); //构造零点
    double z1 = cos(wo); //零点1
    double z2 = sin(wo); //零点2
    double zadd = 2*z1; //z1 + z2
    double zcheng = z1*z1 + z2*z2; //z1*z2
    xian.append(1);
    xian.append(-zadd);
    xian.append(zcheng);
}

void MainWindow::channel_currentIndexChanged(int index)
{
    qDebug()<<"绘图模块"<<index;
    ui->channel->setCurrentIndex(index-1);
//    ui->lineEdit_2->setFocusPolicy(Qt::NoFocus);
//    ui->lineEdit_2->setText(QString::number(index*5+15));

}


void MainWindow::rate_currentIndexChanged(int index)
{
    ui->rate->setCurrentIndex(index-1);
    qDebug()<<"串口模块"<< ui->rate->currentText();

}

void MainWindow::com_currentIndexChanged(int index)
{
    ui->com->setCurrentIndex(index);
    qDebug()<<"com模块"<< ui->com->currentText();

}

//初始bef文件初始化头设置
int MainWindow::set_bdf()
{
    //测试1：8个数据通道+1个事件通道
//    int chns = chall+1;
    //测试2：chns 为数据通道数目  与下面的edfopen_file_writeonly参数有关
    int chns = chall;
    //    qDebug()<<File_save;
    //文件保存名
    QString bdf_name=File_save.left(File_save.length() - 4)+QStringLiteral("_")+QStringLiteral(".bdf");
    char*  bdf_save;
    QByteArray ba = bdf_name.toLatin1();
    bdf_save=ba.data();
    qDebug()<<File_save;
    qDebug()<<bdf_save;
    hdl = edfopen_file_writeonly(bdf_save,EDFLIB_FILETYPE_BDFPLUS, chns+1);

    if(hdl<0)
    {
       printf("error: edfopen_file_writeonly()\n");

       return(1);
     }
     //数据通道采样率设置
     for(int i=0; i<chns; i++)
     {
       if(edf_set_samplefrequency(hdl, i, SMP_FREQ))
       {
         printf("error: edf_set_samplefrequency()\n");

         return(1);
       }
     }
     //事件通道采样率设置
     if(edf_set_samplefrequency(hdl,chall,SMP_FREQ))
     {
       printf("error: edf_set_samplefrequency()\n");
       return(1);
     }


     for(int i=0; i<chns; i++)
     {
         if(edf_set_physical_minimum(hdl, i, -150000.0))
         {
             printf("error: edf_set_physical_minimum()\n");

             return(1);
         }
     }
     if(edf_set_physical_minimum(hdl, chall, 0))
     {
         printf("error: edf_set_physical_maximum()\n");

         return(1);
     }



     for(int i=0; i<chns; i++)
     {
         if(edf_set_physical_maximum(hdl, i, 8388608))
         {
             printf("error: edf_set_physical_maximum()\n");

             return(1);
         }
     }
     if(edf_set_physical_maximum(hdl, chall, 255))
     {
         printf("error: edf_set_physical_maximum()\n");

         return(1);
     }

     for(int i=0; i<chns; i++)
     {
       if(edf_set_digital_maximum(hdl, i, 8388607))
       {
         printf("error: edf_set_digital_maximum()\n");

         return(1);
       }
     }

     if(edf_set_digital_maximum(hdl, chall, 255))
     {
       printf("error: edf_set_digital_maximum()\n");

       return(1);
     }

     for(int i=0; i<chns; i++)
     {
       if(edf_set_digital_minimum(hdl, i, -8388608))
       {
         printf("error: edf_set_digital_minimum()\n");

         return(1);
       }
     }

     if(edf_set_digital_minimum(hdl, chall, 0))
     {
       printf("error: edf_set_digital_minimum()\n");

       return(1);
     }


    //lab轴确定
     for(int i=0; i<chall; i++)
     {
        //参数格式转换
         char*  bdf_lab_now;
         QByteArray ba_lab = bdf_lab[i].toLatin1();
         bdf_lab_now=ba_lab.data();
         qDebug()<<bdf_lab[i];

       if(edf_set_label(hdl,i,bdf_lab_now))
       {
         printf("error: edf_set_physical_minimum()\n");

         return(1);
       }
     }
     //事件通道lab设置
     if(edf_set_label(hdl,chall,"status"))
     {
       printf("error: edf_set_physical_minimum()\n");

       return(1);
     }

     //数据通道物理意义设置
     for(int i=0; i<chns; i++)
     {
       if(edf_set_physical_dimension(hdl, i, "uV"))
       {
         printf("error: edf_set_physical_dimension()\n");

         return(1);
       }
     }
    //事件通道物理意义设置
     if(edf_set_physical_dimension(hdl, chall, ""))
     {
       printf("error: edf_set_physical_dimension()\n");

       return(1);
     }
     return(0);
}

//添加BDF数据
void MainWindow::BDF_data(int Nownum,double eeg)
{
    switch(Nownum)
    {
    case 0:
        addBDFdata(Nownum,eeg,buf1);
        break;
    case 1:
        addBDFdata(Nownum,eeg,buf2);
        break;
    case 2:
        addBDFdata(Nownum,eeg,buf3);
        break;
    case 3:
        addBDFdata(Nownum,eeg,buf4);
        break;
    case 4:
        addBDFdata(Nownum,eeg,buf5);
        break;
    case 5:
        addBDFdata(Nownum,eeg,buf6);
        break;
    case 6:
        addBDFdata(Nownum,eeg,buf7);
        break;
    case 7:
        addBDFdata(Nownum,eeg,buf8);
        break;
//        //21时eeg就是event
//    case 21:
//        addBDFdata(Nownum,eeg,buf21);
//        break;
    }

}
// //now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
//bdf通用代码
void MainWindow::addBDFdata(int ID,int eeg_data,double BDF_Buff_use[1000])
{
    int NowID=ID;
     //添加Event通道事件
    if(NowID == 20)
    {
        if (eventsave ==1 )
        {
            now_label_txt[1]--;
        }
        if(eventsave ==1 && now_label_txt[1]==0)
        {
            BDF_Buff_use[save_sys[NowID]]=eeg_data;
            eventsave=0;
            qDebug()<<"标签ID为"<<eeg_data;
            //now_label_txt[0]++;

        }
        else
        {
              BDF_Buff_use[save_sys[NowID]]=0;
        }
        save_sys[NowID]++;
        if(save_sys[NowID]==1000)
        {
           // qDebug()<<NowID<<"写入"<<BDF_Buff_use[0];
            if(edfwrite_physical_samples(hdl, BDF_Buff_use))
            {
               qDebug()<<NowID<<"error: edfwrite_digital_samples()\n";
            }
            save_sys[NowID]=0;
        }

    }
    else
    {

        BDF_Buff_use[save_sys[NowID]]=eeg_data;
        save_sys[NowID]++;
        if(save_sys[NowID]==1000)
        {
           // qDebug()<<NowID<<"写入"<<BDF_Buff_use[0];
            if(edfwrite_physical_samples(hdl, BDF_Buff_use))
            {
               qDebug()<<NowID<<"error: edfwrite_digital_samples()\n";
            }
            save_sys[NowID]=0;
        }
            if(NowID==chall-1 )
            {
                // qDebug()<<now_label_txt[0];
                if(MetalStart >1)
                {
                    addBDFdata(20,now_label_txt[0],buf21);
                }
                else
                {
                    addBDFdata(20,0,buf21);
                }


            }
    }



//   //写入第七个时，第八个也写入
//    if(NowID==chall-1)
//    {
//        save_sys[NowID+1]++;

//        if(save_sys[NowID+1]==1000)
//        {
//             // qDebug()<<NowID+1<<"写入"<<buf21[0];
//              buf21[2]+=10;

//            if(edfwrite_physical_samples(hdl, buf21))
//            {
//                qDebug()<<NowID<<"error: edfwrite_digital_samples()\n";
//            }
//            save_sys[NowID+1]=0;

//        }
//    }

}
//测试
//ID：通道选择
//eeg_data:eeg数据
//BDF_Buff:写入的物理量数组
//void MainWindow::addBDFdata(int ID,int eeg_data,double BDF_Buff_use[1000])
//{
//    int NowID=ID;
//    if(BDF_Buff_use==buf1)
//    {
//        buf1[save_sys[NowID]]=eeg_data;
//        save_sys[NowID]++;
//        if(save_sys[NowID]==1000)
//        {
//               qDebug()<<BDF_Buff_use[0];
//               qDebug()<<NowID<<"大小是"<<sizeof(buf1)/sizeof (buf1[0]);
//            if(edfwrite_physical_samples(hdl, buf1))
//            {
//                qDebug()<<NowID<<"error: edfwrite_digital_samples()\n";
//            }
//            save_sys[NowID]=0;
//        }
//    }




//    if(BDF_Buff_use==buf2)
//    {
//        buf2[save_sys[NowID]]=eeg_data;
//         save_sys[NowID]++;
//        if(save_sys[NowID]==1000)
//        {
//               qDebug()<<BDF_Buff_use[0];
//               qDebug()<<NowID<<"大小是"<<sizeof(buf1)/sizeof (buf1[0]);
//            if(edfwrite_physical_samples(hdl, buf2))
//            {
//                qDebug()<<NowID<<"error: edfwrite_digital_samples()\n";
//            }
//            save_sys[NowID]=0;
//        }
//    }


//    if(NowID==chall-1)
//    {
//        save_sys[NowID+1]++;

//        if(save_sys[NowID+1]==1000)
//        {
//               qDebug()<<BDF_Buff_use[0];
//               qDebug()<<NowID+1<<"大小是"<<sizeof(buf1)/sizeof (buf1[0]);
//            if(edfwrite_physical_samples(hdl, buf9))
//            {
//                qDebug()<<NowID<<"error: edfwrite_digital_samples()\n";
//            }
//            save_sys[NowID+1]=0;
//        }
//    }

//}


void MainWindow::on_pushButton_3_clicked()
{
    if( ui->pushButton_3->text()=="Start")
    {
        online = 1;
        tcpServer->listen(QHostAddress::Any, 1888);
        if(tcpServer->isListening())
        {
            ui->pushButton_3->setText("Stop");
        }
    }

}



void MainWindow::onDisConnect()
{
    onlien_startsend = 0;
    ui->pushButton_4->setText("Send");
    tcpSocket->disconnectFromHost();
    tcpSocket->deleteLater();

    qDebug()<<"---------------disconnect--------------";
}
void MainWindow::onNewConnect()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadData);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &MainWindow::onDisConnect);
    qDebug()<<"--------------new connect--------------";
}
void MainWindow::onReadData()
{
    QByteArray data = tcpSocket->readAll();
    qDebug()<<"Recieve>>" + data;
}
//1.数据拆分
 void MainWindow:: Onlie_Use_Dada(QString Onlie_UseDada)
 {
   //  qDebug()<<Onlie_UseDada;
     Online_SendData.clear();
        for(int i = 0;i < 8;i++)
        {
            //Onlie_UseDada.mid((8+L)*i+2,L+6)是通道ID+数据+包ID+包尾
            //(8+L)*i+4,L 为纯数据
//            qDebug()<<Onlie_UseDada.mid((8+L)*i+2,L+6);
//            qDebug()<<Onlie_UseDada.mid((8+L)*i+4,L);
            Online_SendData.append(Onlie_UseDada.mid((8+L)*i+4,L));
        }
        for (int j = 0;j < shujuliangint; j ++) {
            for(int k = 0;k < 8; k++)
            {
                // qDebug()<<eeg;
                QString str = Online_SendData[k].mid(j*6,6);
//                int ix=str.toInt(&OK,16);
//                double b = ix;
//                double eeg = ((b-8388608)*0.480772/12);
//                Onlie_SendData_add(eeg,0);
                 Onlie_SendData_add(str,0);
            }
           // now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
            int eeg_event = 0;
            if(j==now_label_txt[1])
            {
                eeg_event =now_label_txt[0];
            }
            Onlie_SendData_add(0,eeg_event);
        }
 }


 //2.data包括脑电数据和标签数据
  void MainWindow:: Onlie_SendData_add(QString EEG_data,int EEG_Event)
  {
      Now_TCP_Data++;
      if(Now_TCP_Data < 9)      {
        // Onlie_SendTCPData+=QString::number(EEG_data,'f',3)+',';
           Onlie_SendTCPData+=EEG_data+',';
      }


      //八通道数据+标签一共9次
      if(Now_TCP_Data == 9)
      {
          Now_TCP_Data=0;
          if(Onlie_SendEvent !=  now_label_txt[0] )
          {
              Online_Num++;
              Onlie_DoneEvent ++;
              qDebug()<<"发送次数为:"<<Onlie_DoneEvent;
          }
          if( Onlie_NowEvent != Onlie_DoneEvent)
          {
              Onlie_NowEvent = Onlie_DoneEvent;
              Onlie_SendEvent =  now_label_txt[0];
              qDebug()<<"发送的标签是："<<Onlie_SendEvent;
              Onlie_SendTCPData+=QString::number(Onlie_SendEvent,10);

          }
          else
          {
               Onlie_SendTCPData+=QString::number(0,10);
          }
          //Onlie_SendTCPData+=QString::number(Onlie_SendEvent,10);
          //qDebug()<<Onlie_SendTCPData.length();


//          if(Online_Num!=1)
//          {
//            // qDebug()<<Online_Num;
//             Onlie_Send(Onlie_SendTCPData);
//          }
          Onlie_Send(Onlie_SendTCPData);

          Onlie_SendTCPData.clear();
      }
     //
  }

  //3.TCP数据发送
  void MainWindow:: Onlie_Send(QString Now_Data)
  {
      if(tcpSocket->isWritable())
      {
         tcpSocket->write(Now_Data.toUtf8());
        // qDebug()<<"发送成功";
      }
  }

void MainWindow::on_pushButton_4_clicked()
{
    onlien_startsend=1;
    ui->pushButton_4->setText("Send-ing");
}
//阻抗检测功能
//停止
void MainWindow::on_pushButton_5_clicked()
{
    QByteArray sendBuf = QByteArray::fromHex("AA 27");

    mySer.write(sendBuf);
     qDebug()<<sendBuf;
}
//阻抗采集
void MainWindow::on_pushButton_6_clicked()
{
    QByteArray sendBuf = QByteArray::fromHex("AA 14");

    mySer.write(sendBuf);
     qDebug()<<sendBuf;


     for(int i = 0; i < chall; ++i)
     {
         ch[i].clear();
     }
     framedata_al.clear();
     QVector<QString> ().swap(framedata_al);
     EEG_IMP_Function = IMP_CONN;
     ui->stackedWidget->setCurrentIndex(1);

}
//数据采集
void MainWindow::on_pushButton_7_clicked()
{
    QByteArray sendBuf = QByteArray::fromHex("AA 18");

    mySer.write(sendBuf);
    qDebug()<<sendBuf;


    for(int i = 0; i < chall; ++i)
    {
        ch[i].clear();
    }
    framedata_al.clear();
    QVector<QString> ().swap(framedata_al);
    EEG_IMP_Function = EEG_CONN;
}
