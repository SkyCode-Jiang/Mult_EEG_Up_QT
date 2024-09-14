#include "devicemicroeeg.h"

DeviceMicroEEG::DeviceMicroEEG(QObject *parent) : QObject(parent)
{
    this->EEG_Port = new QSerialPort();

}

/**********************************************
* @projectName   EEGplylot
* @brief         串口设备建立连接
* @param         void
* @return        void
* @author        JCLStill
**********************************************/
bool DeviceMicroEEG::connectEEG(QString com , QString rate)
{
    bool flag = 0;
    this->com = com;
    this->rate = rate;
    this->EEG_Port->setPortName(com);
    this->EEG_Port->setBaudRate(rate.toInt());
    flag = this->EEG_Port->open(QSerialPort::ReadWrite);
    return  flag;
}

/**********************************************
* @projectName   EEGplylot
* @brief         串口设备断开连接
* @param         void
* @return        void
* @author        JCLStill
**********************************************/
void DeviceMicroEEG::disconnectEEG()
{
//    //清除之前缓存
//    QByteArray rx_data;
//    rx_data = this->EEG_Port->readAll();
    this->EEG_Port->close();
}

/**********************************************
* @projectName   EEGplylot
* @brief         停止采集
* @param         void
* @return        void
* @author        JCLStill
**********************************************/
void DeviceMicroEEG::stopCollect()
{
    QByteArray sendBuf = QByteArray::fromHex("AA 28");
    this->EEG_Port->write(sendBuf);
    qDebug()<<sendBuf;
}


/**********************************************
* @projectName   EEGplylot
* @brief         脑电采集功能
* @param         void
* @return        void
* @author        JCLStill
**********************************************/
void DeviceMicroEEG::startEEGCollect()
{
    QByteArray sendBuf = QByteArray::fromHex("AA 18");
    this->EEG_Port->write(sendBuf);
    qDebug()<<sendBuf;
}


/**********************************************
* @projectName   EEGplylot
* @brief         阻抗采集功能
* @param         void
* @return        void
* @author        JCLStill
**********************************************/

void DeviceMicroEEG::startImpCollect()
{
    QByteArray sendBuf = QByteArray::fromHex("AA 14");
    this->EEG_Port->write(sendBuf);
    qDebug()<<sendBuf;
}

