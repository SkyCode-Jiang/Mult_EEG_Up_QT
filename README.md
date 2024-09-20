# Mult_EEG_Up_QT EEG上位机

QT_Creator 4.11

QT 5.14.1 MinGW

## 数据格式

![image-20240709143702382](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20240709143702382.png)





```
qDebug()<<framedata;  "FF087EDD907EC7BA7EAF807E99487E84617E70727E5F4A7E51727E43CB7E36487E2A2C7E22907E1BC47E18F27E158F7E123E7E14B87E19497E1C907E23737E2AEF7E36777E43AB7E50B07E5D4A20FEAA25000000BB"
```



## Include.h文件修改

```
#define SLAVER_CONN 35         ///< 蓝牙连接间隔
#define CHANNEL_NUM 4           ///< 通道个数
```
## 标签信息


拆分信息
待完善：

拆分：

​	数据与mainwindow

​	在线

界面：

​	标签显示

​	悬停展示数据

