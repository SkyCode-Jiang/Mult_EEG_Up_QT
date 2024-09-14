#ifndef DEVICEMICROEEG_H
#define DEVICEMICROEEG_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QString>
class DeviceMicroEEG : public QObject
{
    Q_OBJECT
public:
    DeviceMicroEEG(QObject *parent,QSerialPort *port):QObject(parent),EEG_Port(port)
    {
        }
    explicit DeviceMicroEEG(QObject *parent = nullptr);
    bool connectEEG(QString com , QString rate); ///< 连接设备

    void disconnectEEG(); ///< 断开连接

    void stopCollect();       ///<停止采集命令

    void startEEGCollect();  ///<EEG采集命令

    void startImpCollect();  ///<阻抗采集命令

    QSerialPort *EEG_Port ;
signals:
private:
    QString com;
    QString rate;




};

#endif // DEVICEMICROEEG_H
