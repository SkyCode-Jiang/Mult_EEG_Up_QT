# Mult_EEG_Up_QT EEG上位机

QT_Creator 4.11

QT 5.14.1 MinGW

类拆分



## 数据格式

![image-20240709143702382](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240709143702382.png)





```
qDebug()<<framedata;  "FF087EDD907EC7BA7EAF807E99487E84617E70727E5F4A7E51727E43CB7E36487E2A2C7E22907E1BC47E18F27E158F7E123E7E14B87E19497E1C907E23737E2AEF7E36777E43AB7E50B07E5D4A20FEAA25000000BB"
```

## 代办：
QMake----->CMake

在线TCP标签队列存储

标签显示





mxk数据格式：

包头+数据+包尾+标签

八通道，中间10次数据，

2+240+2+6