#ifndef EEGSTART_H
#define EEGSTART_H

#include <QWidget>
#include "switchcontrol.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include "ui/MessboxWidget/mymessagebox.h"
#include "mainwindow.h"

namespace Ui {
    class EEGStart;
}

class EEGStart : public QWidget
{
    Q_OBJECT

public:
    SwitchControl *MRSwitchControl;
     MyMessageBox *myMbox;
     MainWindow *My_Plot_Show;
     //判断从机个数
    int Now_Slaver_Num=0;
    int Check_Num[MAX_Slave]={0,0,0,0,0,0,0,0};

    explicit EEGStart(QWidget *parent = nullptr);
    ~EEGStart();

signals:
     void Change_Channel(int channel);
     void Change_Rate(int rate);
     void Change_Com(int rate);
private slots:
    void setMaskRecog();



    void on_pushButton_1_toggled(bool checked);

    void on_pushButton_2_toggled(bool checked);

    void on_pushButton_3_toggled(bool checked);

    void on_pushButton_4_toggled(bool checked);

    void on_pushButton_5_toggled(bool checked);

    void on_pushButton_6_toggled(bool checked);

    void on_pushButton_7_toggled(bool checked);

    void on_pushButton_8_toggled(bool checked);

    void on_comboBox_3_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_comboBox_3_activated(const QString &arg1);

    void on_comboBox_4_currentIndexChanged(int index);



    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_21_clicked();

private:
    Ui::EEGStart *ui;

};

#endif // EEGSTART_H
