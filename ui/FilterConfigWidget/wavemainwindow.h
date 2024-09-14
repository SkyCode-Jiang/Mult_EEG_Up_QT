#ifndef WAVEMAINWINDOW_H
#define WAVEMAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QDebug>
#include "tool/include.h"
namespace Ui {
class waveMainWindow;
}

class waveMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit waveMainWindow(QWidget *parent = nullptr);
    ~waveMainWindow();

signals:
    void updata_send(); //开始主界面滤波函数
    void recovery_send(); //复原为原始波形

private:
    Ui::waveMainWindow *ui;

private:
    QButtonGroup *groupfiltering; //单选按钮分组

public slots:
    void slots_filtering(); //声明按钮组槽函数

public:
    //filtering_flag 1 ：低通  2高通  3陷波
    int filtering_flag = 0;
    int low_f = 0;
    int high_f = 0;
    int notch_f = 50;//默认为50hz陷波
    int max_frq = 500;

private slots:
    void on_lowEdit_textChanged(const QString &arg1);
    void on_highEdit_textChanged(const QString &arg1);
    void on_notchEdit_textChanged(const QString &arg1);

};

#endif // WAVEMAINWINDOW_H
