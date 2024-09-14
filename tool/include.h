#ifndef INCLUDE_H
#define INCLUDE_H
//bdf文件存储
#define SMP_FREQ  (1000)
//最大从机数
#define MAX_Slave 20
#define BDF_LABLE_DATA 20
#define BDF_Save 21
//滤波用
#define SAMPLE 1000
#define LOWPASS_FIR  1
#define HIGHPASS_FIR 2
#define NOTCH_FIR 3
#define FIR_ORDER 500 ///< 滤波器阶数

//抽象为datastruct->eegdata.h
//// 数据计算 以字符串
//// 传输包头——（通道包头-通道数据-通道包尾）*N-传输包尾
///********************************************************************************************************/
///****************************************传输包的数据头****************************************************/
//#define SLAVER_CONN 35        ///< 蓝牙连接间隔
//#define CHANNEL_NUM 4           ///< 通道个数
//#define PACKET_TRAN_HEAD_TXT  "" ///< 传输包尾长度
//#define PACKET_TRAN_HEAD_LEN   0 ///< 传输包头长度
///********************************************************************************************************/
///****************************************传输包的通道数据**************************************************/
//#define PACKET_CHAN_HEAD_TXT "FF01" ///< 通道包头内容
//#define PACKET_CHAN_HEAD_LEN  4     ///< 通道包头长度

//#define PACKET_CHAN_SINGLE_EEG_LEN 6                               ///< 单个数据长度 3*2
//#define PACKET_CHAN_CHANNEL_EEG_LEN  (SLAVER_CONN*PACKET_CHAN_SINGLE_EEG_LEN)  ///<   每个通道数据包长度
//#define PACKET_CHAN_EEG_OFFSET 4    ///< 通道数据开始位置在通道包内的偏移量

//#define PACKET_CHAN_TAIL_TXT "A3FE" ///< 通道包尾内容
//#define PACKET_CHAN_TAIL_LEN 4      ///< 通道包尾长度

//#define PACKET_CHALL_ONE_LEN (PACKET_CHAN_HEAD_LEN+PACKET_CHAN_CHANNEL_EEG_LEN+PACKET_CHAN_TAIL_LEN)
//#define PACKET_CHAN_LEN  ( PACKET_CHALL_ONE_LEN * CHANNEL_NUM )   ///< 传输数据包数据：(通道头+通道数据+通道尾)*通道数目
///********************************************************************************************************/
///********************************************传输包的数据尾************************************************/
//#define PACKET_TRAN_TAIL_TXT  "AA25000000BB" ///< 传输包尾内容
//#define PACKET_TRAN_TAIL_LEN  12             ///< 传输包尾长度
//#define PACKET_LABEL_TXT_INDEX    4   ///< 标签内容在包尾的索引 AA2501
//#define PACKET_LABEL_LOCAL_INDEX  8  ///< 标签位置在包尾的索引  AA25000001
//#define PACKET_LABEL_LOCAL_TXT   2  ///< 标签内容和位置的长度
//#define PACKET_TRAN_LEN (PACKET_TRAN_HEAD_LEN+PACKET_CHAN_LEN+PACKET_TRAN_TAIL_LEN) ///< 传输包的总大小

//#define PACKET_TRAN_LEN_HEX (PACKET_TRAN_LEN/2) ///< 0x11 为2个字符 但为1个16进制

///********************************************************************************************************/
///*******************************传输包的不变数据头和数据尾，用以完整性判断**************************************/
//#define PACKET_TRAN_START "FF"
//#define PACHET_TRAN_START_LEN 2
//#define PACKET_TRAN_STOP "BB"
//#define PACHET_TRAN_STOP_LEN 2
////辅助判断的，包中一快不变的字符串 内容--位置--长度
//#define PACKET_TRAN_SYMBOL_TXT "FEAA"
//#define PACHET_TRAN_SYMBOL_LOC PACKET_CHAN_LEN-2
//#define PACHET_TRAN_SYMBOL_LEN 4
////通道ID的 偏移 长度
//#define PACHERT_CHANNEL_ID_OFFSET 2
//#define PACHERT_CHANNEL_ID_LEN   2



#endif // INCLUDE_H
