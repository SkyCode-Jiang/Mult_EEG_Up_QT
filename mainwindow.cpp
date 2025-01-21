#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <iostream>
#include <QTextStream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thread1 = new QThread();
    myBdfData = new BdfData(this);
    myFilterTool = new  FilterTool(this);
    myMicroEEG = new DeviceMicroEEG(this);


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
    ui->stackedWidget->setCurrentIndex(2);



    showMaximized();//全屏显示


    //(1. 串口打开or关闭:
    ui->serial->setIconSize(QSize(50,50));
    ui->serial->setIcon(QIcon(":/icons/break off.png"));

    //(2. 波特率设置:
   // rate = "115200";
    rate = "3000000";


    //(3. 串口端口设置:
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
     {
        ui->com->addItem(info.portName());
     }


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


    ui->y_set->addItem("0.0001");
    ui->y_set->addItem("0.1");
    ui->y_set->addItem("1");
    ui->y_set->addItem("2");
    ui->y_set->addItem("5");
    //   ui->y_set->addItem("7");
    ui->y_set->addItem("10");


    //(7. 数据位

    Show_Mes=new MyMessageBox();
    qDebug()<<"初始化完成";

    Imp10k=new MyMessageBox();
    Imp10k->setIcon("init");
    Imp10k->setBodyText("阻抗已经小于10K");
    Imp10k->setButtonText("确定");



//     set_filter(20,0,0,0,500);
    //Show_Mes->exec();
   // Show_Mes->show();



}




bool ok;
int hadlaber = 0;
int devicename = 0;



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
        myBdfData->save_sys[i]=0;
    }

    for (int i=0;i<1000;i++)
    {
//        myBdfData->buf21[i]=0;
          myBdfData->bdfData.EEG_BDF_data[BDF_Save - 1][i] = 0;
    }

    tcpServer = new QTcpServer();//监听套接字
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadData);
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

    //打开串口：
       if(serial_flag == false)
       {

           if (com.isNull() || rate.isNull() || (chall ==0 ))
           {
                //ui->show_all->setText("提前完成相应设置");
                QMessageBox::information(this,"提示信息","提前完成相应设置");
                serial_flag = false;
                ui->serial->setIconSize(QSize(50,50));
                ui->serial->setIcon(QIcon(":/icons/break off.png"));
           }
           else
           {
               bool flag;
               flag = myMicroEEG->connectEEG(com,rate); // 连接设备
               connect(myMicroEEG->EEG_Port,&QSerialPort::readyRead , this, &MainWindow::displayData);

               //显示设置：
               QString all;
               if(flag)
               {
                   all = "串口号设置为："+ com + "\r" + "波特率设置为：" + rate
                           + "\r" + "通道数为："+ channel + "\r" +"串口打开成功";
                   serial_flag = true;
                   ui->serial->setIconSize(QSize(50,50));
                   ui->serial->setIcon(QIcon(":/icons/connect.png"));
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
                   ui->serial->setIcon(QIcon(":/icons/break off.png"));
               }
               //ui->show_all->setText(all);
              // QMessageBox::information(this,"提示信息",all);
           }
       }

       //关闭串口：
       else
       {

           mPlot->replot();
           for(int i =0; i<all_lab.size();++i)
           {

               all_lab.at(i)->position->setCoords(-100, 0);
               all_lab.at(i)->setText("");
           }
           m_vline->point1->setCoords(0, 0);
           m_vline->point2->setCoords(0, 1);
           cha_lab= "";
           timerx = 0; //计数测试用
           mPlot->replot();
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
           ui->serial->setIcon(QIcon(":/icons/break off.png"));

           myMicroEEG->disconnectEEG();
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
           ui->star->setIcon(QIcon(":/icons/star.png"));
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

        myMicroEEG->disconnectEEG();
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

        myMicroEEG->connectEEG(com , rate);
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
            ui->r_g->setIcon(QIcon(":/icons/red.png"));
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
            if(myBdfData->bdf_lab_set == true)
            {
                myBdfData->set_Bdf_File(File_save);
                myBdfData->set_Chall(chall);
                myBdfData->set_bdf();
            }

            ui->r_g->setIcon(QIcon(":/icons/green.png"));
        }
    }
    else
    {
        save_flag = false;
        //ui->show_all->setText("停止保存");
        //QMessageBox::information(this,"提示信息","停止保存");
        pathx.close();
        edfclose_file(myBdfData->get_Hdl());
       // bdf_lab_set=false;
        ui->r_g->setIcon(QIcon(":/icons/red.png"));
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

            myMicroEEG->disconnectEEG();
            star_flag = true;
            ui->star->setIcon(QIcon(":/icons/stop.png"));
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
            ui->star->setIcon(QIcon(":/icons/star.png"));

            myMicroEEG->connectEEG(com ,rate);
            //-----------打开串口-------------//

//            ui->show->xAxis->setRange(0, x_range);//修改x的显示范围
//            ui->show->yAxis->setRange(-y_range_mm,y_range_mm);
//            for(int i = 0; i<chall; i++)
//            {
//                 ui->show->graph(i)->data().data()->clear();
//                 ui->show->graph(i)->addData(0,V_constant.at(i));
//            }
//                ui->show->replot();
            mPlot->replot();

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


    if(Now_Function==NOW_IMP_SHOW)
    {
        Now_Function=NOW_EEG_SHOW;
        ui->stackedWidget->setCurrentIndex(2);
       // connect(thread1, SIGNAL(started()), mPlot, SLOT(replot()));
      //  disconnect(thread2, SIGNAL(started()),  ui->show, SLOT(replot()));
    }

    else
    {
        Now_Function=NOW_IMP_SHOW;
        ui->stackedWidget->setCurrentIndex(1);
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



//串口接受线程：
void MainWindow::displayData()
{
    //*串口接受方式2*//
    QByteArray rx_data;
    rx_data = myMicroEEG->EEG_Port->readAll(); //串口数据接受
    //  qDebug()<<rx_data;
    //命令分析
    initAnay(rx_data);
    connAnay(rx_data);
    disconnAnay(rx_data);
    //接收数据分析
    dataUartAnny(rx_data);
}

/**********************************************
* @projectName   EEGplylot
* @brief         从机初始化判断
* @param         void
* @return        QString str
* @author        JCLStill
**********************************************/
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
        Show_Mes->setButtonText("设备初始化请稍后");
        //Show_Mes->exec();
        Show_Mes->show();
    }
}

/**********************************************
* @projectName   EEGplylot
* @brief         连接判断
* @param         void
* @return        QString str
* @author        JCLStill
**********************************************/
void MainWindow::connAnay(QString str)
{
    if(str.startsWith("EEG_SM_TIME_RESP"))
    {
        if(conn_pd_flag!=0)
        {

            return;
        }
        disconn_slaver.clear();
       // disconn_slaver="失去连接设备ID为：";
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
/**********************************************
* @projectName   EEGplylot
* @brief         连接断开判断
* @param         void
* @return        QString str
* @author        JCLStill
**********************************************/
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
        disconn_slaver=disconn_slaver+"0X"+str.mid(Local_Loc-2,2)+"号设备"+"  ";
        Show_Mes->setIcon("error");
        Show_Mes->setBodyText("出现错误：失去连接设备ID为："+disconn_slaver);
        Show_Mes->setButtonText("请重启系统");
        Show_Mes->show();
        Dis_Situation.remove(1,Local_Loc+28);

    }

}
/**********************************************
* @projectName   EEGplylot
* @brief         对串口接受到的数据处理
* @param         void
* @return        QByteArray rx_data
* @author        JCLStill
**********************************************/
void MainWindow::dataUartAnny(QByteArray rx_data)
{
    QString framedata;
    for(int i = 0; i<rx_data.length(); ++i)
    {
        framedata_al.append(rx_data.mid(i,1).toHex().trimmed().toUpper());
    }

    //寻找数据头FF/ff，如果找到了数据头，删除数据头前所有数据
    if(flag_bb == 0)
    {
        for(int i = 0 ; i<framedata_al.length(); ++i)
        {
            if(framedata_al.at(i) == PACKET_TRAN_START)
            {
                if(i != 0)
                {
                    framedata_al.remove(0,i);
                }
                else if(i == 0)
                {
                    //  qDebug()<<all2;
                    if(framedata_al.length()>=baseEEGStruct->PACKET_TRAN_LEN_HEX)
                    {
                        for(int i = 0; i<baseEEGStruct->PACKET_TRAN_LEN_HEX; ++i)
                        {
                            framedata = framedata + framedata_al.at(i);   //0x11---> [字符1] [字符1]
                        }
                        framedata_al.remove(0,baseEEGStruct->PACKET_TRAN_LEN_HEX);
                        chuli_muli_new_fir(framedata,baseEEGStruct->PACKET_CHAN_CHANNEL_EEG_LEN);
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
        if(framedata_al.length()>=baseEEGStruct->PACKET_TRAN_LEN_HEX)
        {
            for(int i = 0; i<baseEEGStruct->PACKET_TRAN_LEN_HEX; ++i)
            {
                framedata = framedata + framedata_al.at(i);
            }
            framedata_al.remove(0,baseEEGStruct->PACKET_TRAN_LEN_HEX);
            //  framedata_al.squeeze();
            // QVector<QString> ().swap(framedata_al);
//            qDebug()<<framedata;
            chuli_muli_new_fir(framedata,baseEEGStruct->PACKET_CHAN_CHANNEL_EEG_LEN);
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
    double low_freq = this->wavepage->low_f ;
    double high_freq = this->wavepage->high_f;
    double notch_lowfreq = 45;
    double notch_highfreq = 55;
    int fir_order = FIR_ORDER;
    int fir_type = this->wavepage->filtering_flag;
    myFilterTool->setFilter(low_freq,high_freq,notch_lowfreq,notch_highfreq,fir_order,fir_type);
}

//恢复至原始波形:
void MainWindow::recovery_muli()
{
    qDebug()<<ch[0];
    qDebug()<<ch[0].length();
    fir_flag = false;
}

//test----输入数据量正式版本需要删除:
void MainWindow::on_lineEdit_2_textChanged(const QString &arg1)
{

}


//new-------多通道新格式:



/**********************************************
* @projectName   EEGplylot
* @brief         标签判断
                 now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
* @param         包含标签的字符串
* @return        int
* @author        JCLStill
**********************************************/

int MainWindow::LabAnary(QString str)
{
//     qDebug()<<str;//AA 25  00  00  00  BB
    if(str.mid(baseEEGStruct->PACKET_LABEL_TXT_INDEX,baseEEGStruct->PACKET_LABEL_LOCAL_TXT)=="00")
    {
        return 0 ;
    }
    else
    {
        //标签的位置
        int label_local =str.mid(baseEEGStruct->PACKET_LABEL_LOCAL_INDEX,baseEEGStruct->PACKET_LABEL_LOCAL_TXT).toInt(&ok,16);
        //标签的内容 4分类1，2，3，4
        int label_txt =str.mid(baseEEGStruct->PACKET_LABEL_TXT_INDEX,baseEEGStruct->PACKET_LABEL_LOCAL_TXT).toInt(&ok,16);;
        qDebug()<<"标签解析的是所有标签，包括每次实验时，可能会多出的那个";
        qDebug()<<"First标签解析"<<label_local<<"   "<<label_txt;

//        myBdfData->bdfData.now_label_txt[0] = label_txt;
//        myBdfData->bdfData.now_label_txt[1] = label_local;

//        myBdfData->bdfData.now_label_txt_q = label_txt;
//        myBdfData->bdfData.now_label_txt[1] = label_local;
        bdf_save_label[0] =label_txt;
        bdf_save_label[1]=label_local;
        now_label_txt[0]=label_txt;
        now_label_txt[1]=label_local;
//        myBdfData->MetalStart++;
//        ++操作注意防止数据溢出
        myBdfData->set_MetalStart(2);

        return 1;
    }
}

/**********************************************
* @projectName   EEGplylot
* @brief         数据处理
* @param        rec： 包头+所有通道数据+包尾的数据
*               L： 每个通道数据包长度
* @return        void
* @author        JCLStill
**********************************************/
void MainWindow::chuli_muli_new_fir(QString rec, int L)
{
//   qDebug()<<rec;
//      qDebug()<< shujuliangint*chall + chall*8;
//      qDebug()<< rec.mid((shujuliangint*chall*6 + chall*8),2);
    //判断数据包是否有所用从机数据
    if (rec.mid(baseEEGStruct->PACHET_TRAN_SYMBOL_LOC,baseEEGStruct->PACHET_TRAN_SYMBOL_LEN)!=PACKET_TRAN_SYMBOL_TXT )
    {
        qDebug()<<rec.mid(baseEEGStruct->PACHET_TRAN_SYMBOL_LOC,baseEEGStruct->PACHET_TRAN_SYMBOL_LEN);
        qDebug()<< baseEEGStruct->PACHET_TRAN_SYMBOL_LOC <<"  Not seccess";
        return;
    }

    else
    {
        //标签判断
        //now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
        LabAnary(rec.mid(baseEEGStruct->PACKET_CHAN_LEN ,baseEEGStruct->PACKET_TRAN_TAIL_LEN));

        //在线数据发送标志
        if(online == 1&& onlien_startsend ==1)
        {
            Onlie_Use_Dada(rec);
        }


        if(save_flag == true && flag_y == true)
        {
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
            out << res;
            out.flush();
        }
        //曲线名称图例设置  bdf标签设置
        for(int legset=0;legset<chall;legset++)
        {

            mPlot->setLegen(legset,0,rec.mid(legset*baseEEGStruct->PACKET_CHALL_ONE_LEN+baseEEGStruct->PACHERT_CHANNEL_ID_OFFSET,baseEEGStruct->PACHERT_CHANNEL_ID_LEN)); //设置名称
            lable_order(mPlot,mPlot->RectList[legset]->axis(QCPAxis::atRight),rec.mid(legset*baseEEGStruct->PACKET_CHALL_ONE_LEN+baseEEGStruct->PACHERT_CHANNEL_ID_OFFSET,baseEEGStruct->PACHERT_CHANNEL_ID_LEN)+"\r\n设备",mPlot->RectList[legset]);
            if(myBdfData->bdf_lab_flag==false)
            {
                myBdfData->bdfData.bdf_lab.append(rec.mid(legset*baseEEGStruct->PACKET_CHALL_ONE_LEN+baseEEGStruct->PACHERT_CHANNEL_ID_OFFSET,baseEEGStruct->PACHERT_CHANNEL_ID_LEN));
                if(legset==chall-1)
                {
                    myBdfData->bdf_lab_flag=true;
                }
            }

        }
        //标签ID自动添加，获取保存文件名+通道ID
        if(save_path==true &&  myBdfData->bdf_lab_flag==true && myBdfData->bdf_lab_set==false )
        {
             //set_bdf();//打开一个bdf文件并设置
             myBdfData->bdf_lab_set=true;
        }

        //(3.数据处理:
        if(flag_y == true)
        {
            for(auto i_num = 0; i_num<chall; ++i_num)
            {
                int now_channel=(i_num+1)%chall;
//                qDebug()<<"now_channel"<< now_channel<<"i_num"<<i_num;
                //检测丢包:3.4屏蔽功能
                //data_verification_all(rec.mid(2,2),rec.mid(all3,2));

                cha_data = rec.mid(baseEEGStruct->PACKET_CHAN_EEG_OFFSET,L);
                //qDebug()<<cha_data;

                for(int i = 0; i<(cha_data.length()/baseEEGStruct->PACKET_CHAN_SINGLE_EEG_LEN); ++i)
                {
                    //timerx之后修改为与采样率
                    time_x[now_channel]= time_x[now_channel]+0.001;

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

                    int k = i*baseEEGStruct->PACKET_CHAN_SINGLE_EEG_LEN;
                    QString str = cha_data.mid(k,baseEEGStruct->PACKET_CHAN_SINGLE_EEG_LEN);
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
                        //                        if(now_label_txt[0]!=0)
                        //                        if(myBdfData->bdfData.now_label_txt[1]!=0  )
                        if(bdf_save_label[0] != 0)
                        {
                            int use_size = myBdfData->bdfData.now_label_local_q.size();
                            qDebug()<<"SIZE"<<myBdfData->bdfData.now_label_local_q.size();
//                            if (use_size >=1 && myBdfData->bdfData.now_label_local_q[0] != 0)
//                            {
//                                myBdfData->bdfData.now_label_txt_q.append(bdf_save_label[0]);
//                                myBdfData->bdfData.now_label_local_q.append(bdf_save_label[1]);
//                            }else
//                            {
                                myBdfData->bdfData.now_label_txt_q.append( bdf_save_label[0]);
                                myBdfData->bdfData.now_label_local_q.append(bdf_save_label[1]);
//                            }
                             qDebug()<<"SIZE"<<myBdfData->bdfData.now_label_local_q.size();
                            myBdfData->startsave = 1;
                            myBdfData->eventsave += 1;
                            bdf_save_label[0] = 0;
//                            bdf_save_label[1] = 0;

                        }

                        myBdfData->BDF_data(i_num,(eeg));
                    }
                    //******************************************************
                    //*********************************************脑电显示**
                    if(Now_Function==NOW_EEG_SHOW)
                    {
                       plot_EEG_Fun(eeg,i_num,now_channel);
                        // mPlot->addData(i_num,0,time_x[now_channel],eeg);
                       // Calibration_value(i_num,(ui->y_set->currentText().toInt()*1000+ MAX_Shou[i_num])-(MIN_Shou[i_num]-(ui->y_set->currentText().toInt()*1000)));
                    }
                    //******************************************************
                    //*********************************************阻抗检测**
                    if(Now_Function==NOW_IMP_SHOW) {
                        plot_IMP_Fun(eeg,i_num+1,0);
                    }

                }
                rec.remove(0,(baseEEGStruct->PACKET_CHALL_ONE_LEN));
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

    }
}
/**********************************************
* @projectName   EEGplylot
* @brief         设置图例名字与阻抗显示界面名字
* @param         void
* @return        void
* @author        JCLStill
**********************************************/

void MainWindow::lable_order(QCustomPlot *my_customplot,QCPAxis *yAxis, QString label, QCPAxisRect *rect)
{
    if (hadlaber < chall)
    {
        static int  label_num = 0;
        yAxisLabel[label_num] = new QCPItemText(my_customplot);
        yAxisLabel[label_num]->position->setParentAnchor(NULL);
        yAxisLabel[label_num]->setText(label);
        yAxisLabel[label_num]->setRotation(0);
        yAxisLabel[label_num]->position->setAxisRect(rect);
        yAxisLabel[label_num]->setClipToAxisRect(false);
        //x is absolut position
        yAxisLabel[label_num]->position->setTypeX(QCPItemPosition::ptAbsolute);
        //y is relativ to AxesRect
        yAxisLabel[label_num]->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
        yAxisLabel[label_num]->position->setAxes(0, yAxis);
        yAxisLabel[label_num]->position->setCoords(20,0.5);
        label_num++;


        if(label_num==channel.toInt(&ok,10))
        {
            label_num=0;
        }

//        QString labelName = QString("SlaveIR_%1").arg(hadlaber+1);
//        QLabel *setlabel = this->findChild<QLabel *>(labelName);
//        setlabel->setText(label);


        hadlaber++;
    }
    //阻抗显示界面名字
//    if(devicename<chall)
//    {
//        QString labelName = QString("SlaveIR_%1").arg(i+1);
//        QLabel *setlabel = this->findChild<QLabel *>(labelName);
//        setlabel->setText(label);
//    }

}
/**********************************************
* @projectName   EEGplylot
* @brief         EEG计算绘制
* @param          数据，当前第几个通道,
* @return        void
* @author        JCLStill
**********************************************/
void MainWindow::plot_EEG_Fun(double eeg,int i_num,int now_channel )
{
    if(ui->stackedWidget->currentIndex() == 1)
    {
        ui->stackedWidget->setCurrentIndex(2);
    }
    //滤波
    if(fir_flag == true)
    {
        double eeg2 = 0;
        eeg2 = myFilterTool->fir[chall_num+chall]->filter(eeg);

        qDebug()<<eeg2;
        if (eeg2>MAX_Shou[i_num])
        {
            MAX_Shou[i_num]=eeg2;
        }
        if (eeg2<MIN_Shou[i_num])
        {
            MIN_Shou[i_num]=eeg2;
        }
        mPlot->RectList[i_num]->axis(QCPAxis::atLeft)->setRange(MIN_Shou[i_num]-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+ MAX_Shou[i_num]);//修改y的显示范围
        mPlot->addData(i_num,0,time_x[now_channel],eeg2);

    }
    else
    {
        //    qDebug()<<eeg;
        ui->show->yAxis->setRange(Min-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+Max);//修改y的显示范围
        mPlot->RectList[i_num]->axis(QCPAxis::atLeft)->setRange(MIN_Shou[i_num]-(ui->y_set->currentText().toInt()*1000), ui->y_set->currentText().toInt()*1000+ MAX_Shou[i_num]);
        mPlot->addData(i_num,0,time_x[now_channel],eeg);
        Calibration_value(i_num,(ui->y_set->currentText().toInt()*1000+ Max)-(Min-(ui->y_set->currentText().toInt()*1000)));
    }
}
/**********************************************
* @projectName   EEGplylot
* @brief         阻抗计算显示
* @param         数据，当前第几个通道,
* @return        void
* @author        JCLStill
**********************************************/
void MainWindow::plot_IMP_Fun(double eeg,int i_num,int now_channel)
{
    if(ui->stackedWidget->currentIndex() == 2)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    QString labelName = QString("SlaveIR_%1").arg(i_num);
    QLabel *label = this->findChild<QLabel *>(labelName);

    double NOW_Implace = (eeg/2*16777216)/4.033/6/1000000;
    NOW_Implace =  (NOW_Implace-851661.844)/1000;
    NOW_Implace = (NOW_Implace + 830) /2;   //831
//    显示阻抗值
    qDebug()<<labelName <<"阻抗: " << NOW_Implace;
    if(NOW_Implace>0.5)
    {
        LAST_Implace = NOW_Implace;
        int ColIndex =int(NOW_Implace/10);
        if (ColIndex>=5)
        {
            ColIndex=5;
        }
        if (NOW_Implace<=10)
        {
            ColIndex=0;
        }
//        qDebug()<<NOW_Implace;
//        qDebug()<<ColIndex;

        QString col = QString("QLabel{background-color:%1;}").arg(Allcol[ColIndex]);
//        ui->SlaveIR_1->setStyleSheet(col);
        label->setStyleSheet(col);
        if(IMPPD == 1 && NOW_Implace>10 )
        {
            // Imp10k->hide();
            // IMPPD = 0;
        }

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



void MainWindow::channel_currentIndexChanged(int index)
{
    qDebug()<<"通道模块"<<index;
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
    if(data == "start")
    {
        //手动发出clicked信号
        ui->serial->click();
        qDebug()<<"打开串口";
    }
    if(data == "stop")
    {
        //手动发出clicked信号
        ui->serial->click();
        qDebug()<<"关闭串口";
    }
    if(data == "start_send")
    {

        ui->pushButton_4->click();
         qDebug()<<"开始发送数据";
    }
    if(data == "stop_send")
    {
        ui->pushButton_4->click();
         qDebug()<<"停止发送数据";
    }
}
//1.数据拆分
 void MainWindow:: Onlie_Use_Dada(QString Onlie_UseDada)
 {
     //  qDebug()<<Onlie_UseDada;
     Online_SendData.clear();
     for(int i = 0;i < chall;i++)
     {
         //(PACKET_CHALL_ONE_LEN)*i+2,PACKET_CHAN_CHANNEL_EEG_LEN是通道ID+数据+包ID+包尾
         //(PACKET_CHALL_ONE_LEN)*i+PACKET_CHAN_EEG_OFFSET,PACKET_CHAN_CHANNEL_EEG_LEN 为纯数据
         Online_SendData.append(Onlie_UseDada.mid((baseEEGStruct->PACKET_CHALL_ONE_LEN)*i+baseEEGStruct->PACKET_CHAN_EEG_OFFSET,baseEEGStruct->PACKET_CHAN_CHANNEL_EEG_LEN));
     }
     for (int j = 0;j < shujuliangint; j++) {
         for(int k = 0;k < chall; k++)
         {
             // qDebug()<<eeg;
             QString str = Online_SendData[k].mid(j*6,6);
             Onlie_SendData_add(str,0);
         }
         // now_label_txt[0] 代表内容 now_label_txt[1] 代表位置
         int eeg_event = 0;
//         if(j== now_label_txt[1])
//         {
//             eeg_event = now_label_txt[0];
//         }
         //这块需要修改
         if(j== now_label_txt[1])
         {
             eeg_event = now_label_txt[0];
         }
         Onlie_SendData_add(0,eeg_event);
     }
 }


 //2.data包括脑电数据和标签数据
  void MainWindow:: Onlie_SendData_add(QString EEG_data,int EEG_Event)
  {
      Now_TCP_Data++;
      if(Now_TCP_Data < chall+1)      {
        // Onlie_SendTCPData+=QString::number(EEG_data,'f',3)+',';
           Onlie_SendTCPData+=EEG_data+',';
      }


      //八通道数据+标签一共9次
      if(Now_TCP_Data == chall+1)
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
//              Onlie_SendTCPData+=QString::number(Onlie_SendEvent,10)+";";
              if( Onlie_SendEvent<= 0x0F)
              {
                Onlie_SendTCPData+='0'+QString::number(Onlie_SendEvent,16);
              }else
              {
                 Onlie_SendTCPData+=QString::number(Onlie_SendEvent,16);
              }


              qDebug()<<"发送的带标签数据是："<<Onlie_SendTCPData;

          }
          else
          {
//               Onlie_SendTCPData+=QString::number(0,10)+QString::number(0,10)+";";
               Onlie_SendTCPData+=QString::number(0,10)+QString::number(0,10);
          }
          //Onlie_SendTCPData+=QString::number(Onlie_SendEvent,10);
          //qDebug()<<Onlie_SendTCPData.length();


//          if(Online_Num!=1)
//          {
//            // qDebug()<<Online_Num;
//             Onlie_Send(Onlie_SendTCPData);
//          }

//每次发送的数据
//         qDebug()<<Onlie_SendTCPData;

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
    if( onlien_startsend == 0 )
    {
        onlien_startsend=1;
        ui->pushButton_4->setText("Send-ing");
    }
    else
    {
        onlien_startsend=0;
        ui->pushButton_4->setText("Send-stop");
    }

}
//阻抗检测功能

//EEG采集
void MainWindow::on_pushButton_7_clicked()
{
    //清除串口缓存
    QByteArray rx_data;
    rx_data = myMicroEEG->EEG_Port->readAll(); //串口数据接受
    connect(myMicroEEG->EEG_Port,&QSerialPort::readyRead , this, &MainWindow::displayData);
    //清除变量缓存
    mPlot->replot();
    for(int i = 0; i < chall; ++i)
    {
        ch[i].clear();
    }
    framedata_al.clear();
    QVector<QString> ().swap(framedata_al);
    //功能设置
    EEG_IMP_Function = EEG_CONN;
    Now_Function = NOW_EEG_SHOW;
    ui->stackedWidget->setCurrentIndex(2);
    //发送指令--EEG采集
    myMicroEEG->startEEGCollect();
}
//阻抗采集
void MainWindow::on_pushButton_6_clicked()
{

      //清除串口缓存
    QByteArray rx_data;
    rx_data = myMicroEEG->EEG_Port->readAll(); //串口数据接受
    connect(myMicroEEG->EEG_Port,&QSerialPort::readyRead , this, &MainWindow::displayData);
    //清除变量缓存
     for(int i = 0; i < chall; ++i)
     {
         ch[i].clear();
     }
     framedata_al.clear();
     QVector<QString> ().swap(framedata_al);
    //功能设置
     EEG_IMP_Function = IMP_CONN;
     Now_Function = NOW_IMP_SHOW;
     ui->stackedWidget->setCurrentIndex(1);
    //发送指令--阻抗检测
    myMicroEEG->startImpCollect();

}

//停止
void MainWindow::on_pushButton_5_clicked()
{
    //发送指令--停止采集
    myMicroEEG->stopCollect();
    //清除变量缓存
     for(int i = 0; i < chall; ++i)
     {
         ch[i].clear();
     }
     framedata_al.clear();
     QVector<QString> ().swap(framedata_al);
     chall_num = 0;
     flag_bb = 0;
     disconnect(myMicroEEG->EEG_Port,&QSerialPort::readyRead , this, &MainWindow::displayData);

}

//1).多通道数据格式设置:
void MainWindow::data_sample()
{
    qDebug()<<shujuliangint;

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
        mPlot->RectList[i]->axis(QCPAxis::atBottom)->setRange(-0.05, x_range);
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
        mPlot->RectList[i]->axis(QCPAxis::atBottom)->setRange(-0.2, x_range);
    }



    mPlot->replot();
    ui->show->replot();
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



//Y轴定标值计算    每毫米的计算 微V计算
void MainWindow::Calibration_value(int Chall_show_value,double unint_mv)
{
    double value;
    QString Calibration_value_show;
    if(Now_Function==NOW_IMP_SHOW)
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

void MainWindow::setChannelChooseFirst()
{
    ui->channel->setCurrentIndex(10);
    colour.append(QPen(QColor(255,128,128))); //ch2
    colour.append(QPen(QColor(255,0,0))); //ch3
    colour.append(QPen(QColor(0,255,0))); //ch4
    colour.append(QPen(QColor(255,255,255))); //ch2
    colour.append(QPen(QColor(255,255,0))); //ch3

    colour.append(QPen(QColor(128,0,255))); //ch2
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
    colour.append(QPen(QColor(0,64,0))); //ch3
    colour.append(QPen(QColor(255,0,255))); //ch4
    colour.append(QPen(QColor(128,0,255))); //ch2
    colour.append(QPen(QColor(79,79,79))); //ch3
}

