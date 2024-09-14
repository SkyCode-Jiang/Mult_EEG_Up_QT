#include "eegdata.h"

EEGData::EEGData()
{

}

EEGData::EEGData(int Slaver_num , int Channel_num)
{
    this->SLAVER_CONN = Slaver_num;        ///< 蓝牙连接间隔
    this->CHANNEL_NUM = Channel_num ;          ///< 通道个数

    this->PACKET_TRAN_HEAD_LEN  = 0; ///< 传输包头长度
    /********************************************************************************************************/
    /****************************************传输包的通道数据**************************************************/

    this-> PACKET_CHAN_HEAD_LEN = 4;     ///< 通道包头长度

    this-> PACKET_CHAN_SINGLE_EEG_LEN = 6;                               ///< 单个数据长度 3*2
    this-> PACKET_CHAN_CHANNEL_EEG_LEN = (this->SLAVER_CONN*this->PACKET_CHAN_SINGLE_EEG_LEN);  ///<   每个通道数据包长度
    this->  PACKET_CHAN_EEG_OFFSET =  4;    ///< 通道数据开始位置在通道包内的偏移量

    #define PACKET_CHAN_TAIL_TXT "A3FE" ///< 通道包尾内容
    this-> PACKET_CHAN_TAIL_LEN = 4;      ///< 通道包尾长度

    this-> PACKET_CHALL_ONE_LEN = (this->PACKET_CHAN_HEAD_LEN+this->PACKET_CHAN_CHANNEL_EEG_LEN+this->PACKET_CHAN_TAIL_LEN);
    this-> PACKET_CHAN_LEN = ( this->PACKET_CHALL_ONE_LEN * this->CHANNEL_NUM );   ///< 传输数据包数据：(通道头+通道数据+通道尾)*通道数目
    /********************************************************************************************************/
    /********************************************传输包的数据尾************************************************/
    #define PACKET_TRAN_TAIL_TXT  "AA25000000BB" ///< 传输包尾内容
    this-> PACKET_TRAN_TAIL_LEN = 12;             ///< 传输包尾长度
    this-> PACKET_LABEL_TXT_INDEX  =  4;   ///< 标签内容在包尾的索引 AA2501
    this-> PACKET_LABEL_LOCAL_INDEX = 8;  ///< 标签位置在包尾的索引  AA25000001
    this-> PACKET_LABEL_LOCAL_TXT  = 2;  ///< 标签内容和位置的长度
    this-> PACKET_TRAN_LEN = (this->PACKET_TRAN_HEAD_LEN+this->PACKET_CHAN_LEN+this->PACKET_TRAN_TAIL_LEN); ///< 传输包的总大小

    this-> PACKET_TRAN_LEN_HEX = (this->PACKET_TRAN_LEN/2); ///< 0x11 为2个字符 但为1个16进制

    /********************************************************************************************************/
    /*******************************传输包的不变数据头和数据尾，用以完整性判断**************************************/
    #define PACKET_TRAN_START "FF"
    this-> PACHET_TRAN_START_LEN = 2;
    #define PACKET_TRAN_STOP "BB"
    this-> PACHET_TRAN_STOP_LEN = 2;
    //辅助判断的，包中一快不变的字符串 内容--位置--长度
    #define PACKET_TRAN_SYMBOL_TXT "FEAA"
    this-> PACHET_TRAN_SYMBOL_LOC = this->PACKET_CHAN_LEN-2;
    this-> PACHET_TRAN_SYMBOL_LEN = 4;
    //通道ID的 偏移 长度
    this-> PACHERT_CHANNEL_ID_OFFSET = 2;
    this-> PACHERT_CHANNEL_ID_LEN  = 2;
}
