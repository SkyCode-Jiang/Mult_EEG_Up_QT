#ifndef EEGDATA_H
#define EEGDATA_H

#include <QObject>
#define PACKET_TRAN_HEAD_TXT  "" ///< 传输包尾长度
#define PACKET_CHAN_HEAD_TXT "FF01" ///< 通道包头内容
#define PACKET_CHAN_TAIL_TXT "A3FE" ///< 通道包尾内容
#define PACKET_TRAN_TAIL_TXT  "AA25000000BB" ///< 传输包尾内容
#define PACKET_TRAN_START "FF"
#define PACKET_TRAN_STOP "BB"
#define PACKET_TRAN_SYMBOL_TXT "FEAA"

class EEGData
{
public:
    EEGData();
    EEGData(int Slaver_num , int Channel_num);

    // 数据计算 以字符串
    // 传输包头——（通道包头-通道数据-通道包尾）*N-传输包尾
    /********************************************************************************************************/
    /****************************************传输包的数据头****************************************************/
    int SLAVER_CONN ;        ///< 蓝牙连接间隔
    int CHANNEL_NUM  ;          ///< 通道个数
//    #define PACKET_TRAN_HEAD_TXT  "" ///< 传输包尾长度
    int PACKET_TRAN_HEAD_LEN ; ///< 传输包头长度
    /********************************************************************************************************/
    /****************************************传输包的通道数据**************************************************/
//    #define PACKET_CHAN_HEAD_TXT "FF01" ///< 通道包头内容
    int PACKET_CHAN_HEAD_LEN ;     ///< 通道包头长度

    int PACKET_CHAN_SINGLE_EEG_LEN ;                               ///< 单个数据长度 3*2
    int PACKET_CHAN_CHANNEL_EEG_LEN ;  ///<   每个通道数据包长度
    int  PACKET_CHAN_EEG_OFFSET;    ///< 通道数据开始位置在通道包内的偏移量

//    #define PACKET_CHAN_TAIL_TXT "A3FE" ///< 通道包尾内容
    int PACKET_CHAN_TAIL_LEN ;      ///< 通道包尾长度

    int PACKET_CHALL_ONE_LEN ;
    int PACKET_CHAN_LEN;   ///< 传输数据包数据：(通道头+通道数据+通道尾)*通道数目
    /********************************************************************************************************/
    /********************************************传输包的数据尾************************************************/
//    #define PACKET_TRAN_TAIL_TXT  "AA25000000BB" ///< 传输包尾内容
    int PACKET_TRAN_TAIL_LEN ;             ///< 传输包尾长度
    int PACKET_LABEL_TXT_INDEX  ;   ///< 标签内容在包尾的索引 AA2501
    int PACKET_LABEL_LOCAL_INDEX ;  ///< 标签位置在包尾的索引  AA25000001
    int PACKET_LABEL_LOCAL_TXT ;  ///< 标签内容和位置的长度
    int PACKET_TRAN_LEN; ///< 传输包的总大小

    int PACKET_TRAN_LEN_HEX; ///< 0x11 为2个字符 但为1个16进制

    /********************************************************************************************************/
    /*******************************传输包的不变数据头和数据尾，用以完整性判断**************************************/
//    #define PACKET_TRAN_START "FF"
    int PACHET_TRAN_START_LEN ;
//    #define PACKET_TRAN_STOP "BB"
    int PACHET_TRAN_STOP_LEN;
    //辅助判断的，包中一快不变的字符串 内容--位置--长度
//    #define PACKET_TRAN_SYMBOL_TXT "FEAA"
    int PACHET_TRAN_SYMBOL_LOC ;
    int PACHET_TRAN_SYMBOL_LEN ;
    //通道ID的 偏移 长度
    int PACHERT_CHANNEL_ID_OFFSET;
    int PACHERT_CHANNEL_ID_LEN ;
};

#endif // EEGDATA_H
