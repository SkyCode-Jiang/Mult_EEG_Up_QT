#include "bdfdata.h"

BdfData::BdfData(QObject *parent) : QObject(parent)
{
//    this->bdfData.now_label_txt_q.append(0);
//    this->bdfData.now_label_local_q .append(0);
}
//初始bef文件初始化头设置
int BdfData::set_bdf()
{
    //测试1：8个数据通道+1个事件通道
//    int chns = chall+1;
    //测试2：chns 为数据通道数目  与下面的edfopen_file_writeonly参数有关
    int chns = this->chall;
    //    qDebug()<<File_save;
    //文件保存名
    QString bdf_name=this->bdf_File_Save.left(this->bdf_File_Save.length() - 4)+QStringLiteral("_")+QStringLiteral(".bdf");
    char*  bdf_save;
    QByteArray ba = bdf_name.toLatin1();
    bdf_save=ba.data();
    qDebug()<<this->bdf_File_Save;
    qDebug()<<bdf_save;

    this->hdl = edfopen_file_writeonly(bdf_save,EDFLIB_FILETYPE_BDFPLUS, chns+1);

    if(this->hdl<0)
    {
       printf("error: edfopen_file_writeonly()\n");

       return(1);
     }
     //数据通道采样率设置
     for(int i=0; i<chns; i++)
     {
       if(edf_set_samplefrequency(this->hdl, i, SMP_FREQ))
       {
         printf("error: edf_set_samplefrequency()\n");

         return(1);
       }
     }
     //事件通道采样率设置
     if(edf_set_samplefrequency(this->hdl,chall,SMP_FREQ))
     {
       printf("error: edf_set_samplefrequency()\n");
       return(1);
     }


     for(int i=0; i<chns; i++)
     {
         if(edf_set_physical_minimum(this->hdl, i, -150000.0))
         {
             printf("error: edf_set_physical_minimum()\n");

             return(1);
         }
     }
     if(edf_set_physical_minimum(this->hdl, chall, 0))
     {
         printf("error: edf_set_physical_maximum()\n");

         return(1);
     }



     for(int i=0; i<chns; i++)
     {
         if(edf_set_physical_maximum(this->hdl, i, 8388608))
         {
             printf("error: edf_set_physical_maximum()\n");

             return(1);
         }
     }
     if(edf_set_physical_maximum(this->hdl, chall, 255))
     {
         printf("error: edf_set_physical_maximum()\n");

         return(1);
     }

     for(int i=0; i<chns; i++)
     {
       if(edf_set_digital_maximum(this->hdl, i, 8388607))
       {
         printf("error: edf_set_digital_maximum()\n");

         return(1);
       }
     }

     if(edf_set_digital_maximum(this->hdl, chall, 255))
     {
       printf("error: edf_set_digital_maximum()\n");

       return(1);
     }

     for(int i=0; i<chns; i++)
     {
       if(edf_set_digital_minimum(this->hdl, i, -8388608))
       {
         printf("error: edf_set_digital_minimum()\n");

         return(1);
       }
     }

     if(edf_set_digital_minimum(this->hdl, chall, 0))
     {
       printf("error: edf_set_digital_minimum()\n");

       return(1);
     }


    //lab轴确定
     for(int i=0; i<chall; i++)
     {
        //参数格式转换
         char*  bdf_lab_now;
         QByteArray ba_lab = this->bdfData.bdf_lab[i].toLatin1();
         bdf_lab_now=ba_lab.data();
         qDebug()<<this->bdfData.bdf_lab[i];

       if(edf_set_label(this->hdl,i,bdf_lab_now))
       {
         printf("error: edf_set_physical_minimum()\n");

         return(1);
       }
     }
     //事件通道lab设置
     if(edf_set_label(this->hdl,chall,"status"))
     {
       printf("error: edf_set_physical_minimum()\n");

       return(1);
     }

     //数据通道物理意义设置
     for(int i=0; i<chns; i++)
     {
       if(edf_set_physical_dimension(this->hdl, i, "uV"))
       {
         printf("error: edf_set_physical_dimension()\n");

         return(1);
       }
     }
    //事件通道物理意义设置
     if(edf_set_physical_dimension(this->hdl, chall, ""))
     {
       printf("error: edf_set_physical_dimension()\n");

       return(1);
     }
     return(0);
}


void BdfData::set_Hdl(int hdl)
{
    this->hdl = hdl;
    qDebug()<<"BDF hdl"<<hdl;
}

int BdfData::get_Hdl(void)
{
    return this->hdl;
}

void BdfData::set_Chall(int chall)
{
    this->chall = chall;
    qDebug()<<"BDF chall"<<chall;
}

void BdfData::set_Bdf_File(QString bdf_File_Save)
{
    this->bdf_File_Save = bdf_File_Save;
    qDebug()<<"BDF bdf_File_Save name"<<bdf_File_Save;
}

void BdfData::set_MetalStart(int num)
{
    this->MetalStart = num;
}

//添加BDF数据
void BdfData::BDF_data(int Nownum,double eeg)
{
    addBDFdata(Nownum ,eeg ,this->bdfData.EEG_BDF_data[Nownum]);
}

void BdfData::addlabel_zero(int id,QVector<double> & BDF_Buff_usezero)
{
    if (eventsave >= 1 )
    {
//        qDebug()<<"size****zero********"<<this->bdfData.now_label_local_q.size();
        for(int i=0; i<this->bdfData.now_label_local_q.size(); ++i)
        {
            if(this->bdfData.now_label_local_q[i] == 0)
            {
                BDF_Buff_usezero[save_sys[id]]=this->bdfData.now_label_txt_q[i];
                eventsave = eventsave-1;
                qDebug()<<"标签ID为"<<i<<this->bdfData.now_label_txt_q[i];
                this->bdfData.now_label_local_q.takeAt(i);
                this->bdfData.now_label_txt_q.takeAt(i);
            }

        }
    }
}


 int BdfData::addlabel_common(int id,QVector<double> & BDF_Buff_use_com)
 {
     if(eventsave >=1)
     {
         for(int i=0; i<this->bdfData.now_label_local_q.size(); ++i)
         {
//             qDebug()<<"size****common********"<<this->bdfData.now_label_local_q.size();
             if(this->bdfData.now_label_local_q[i] == 0)
             {
                 eventsave = eventsave-1;
                 int label = this->bdfData.now_label_txt_q[i];
                 qDebug()<<"标签ID为"<<label;
                 this->bdfData.now_label_local_q.takeAt(i);
                 this->bdfData.now_label_txt_q.takeAt(i);
//                  qDebug()<<"删除后的size****common********"<<this->bdfData.now_label_local_q.size();
                 return label;
             }
         }


     }
     return 0;

 }

//bdf通用代码
void BdfData::addBDFdata(int ID,int eeg_data,QVector<double> & BDF_Buff_use)
{
    int NowID=ID;
    std::unique_ptr<double> my_Buffer(new double[SMP_FREQ]) ;
     //添加Event通道事件
    if(NowID == 20)
    {

//        if (eventsave >= 1 && this->bdfData.now_label_local_q[1]==0)
//        {
//            BDF_Buff_use[save_sys[NowID]]=eeg_data;

//            eventsave = eventsave-1;
//            qDebug()<<"标签ID为"<<this->bdfData.now_label_local_q[0];


//        }
        addlabel_zero(NowID,BDF_Buff_use);
        if (eventsave >= 1 )
        {

            for(int i=0; i<this->bdfData.now_label_local_q.size(); ++i){
                this->bdfData.now_label_local_q[i]--;
//                qDebug()<<"查看第 i 个标签是否为0了："<<this->bdfData.now_label_local_q[i];
            }

        }
        //如果标签在对应数据位置非0处，判断now_label_txt是否为0；标签在对应数据位置0处，此时now_label_txt为-1
        BDF_Buff_use[save_sys[NowID]] = addlabel_common(NowID,BDF_Buff_use);

        save_sys[NowID]++;
        if(save_sys[NowID]==SMP_FREQ)
        {

            memcpy(my_Buffer.get(), &BDF_Buff_use[0], BDF_Buff_use.size()*sizeof(double));
           // qDebug()<<NowID<<"写入"<<BDF_Buff_use[0];
            if(edfwrite_physical_samples(this->hdl, my_Buffer.get()))
            {
               qDebug()<<NowID<<"error: edfwrite_digital_samples()\n";
            }
            save_sys[NowID]=0;
        }

    }
    else
    {
        BDF_Buff_use[save_sys[NowID]]=eeg_data;
        save_sys[NowID]++;
        if(save_sys[NowID]==SMP_FREQ)
        {
           // qDebug()<<NowID<<"写入"<<BDF_Buff_use[0];
            memcpy(my_Buffer.get(), &BDF_Buff_use[0], BDF_Buff_use.size()*sizeof(double));
            if(edfwrite_physical_samples(hdl, my_Buffer.get()))
            {
               qDebug()<<NowID<<"error: edfwrite_digital_samples()\n";
            }
            save_sys[NowID]=0;
        }
            if(NowID==chall-1 )
            {
                // qDebug()<<now_label_txt[0];
                if(this->MetalStart == 2)
                {
                    //先到的点，一定比后到的点先打上
                    addBDFdata(BDF_LABLE_DATA,0,this->bdfData.EEG_BDF_data[BDF_LABLE_DATA]);
                }
                else
                {
                    if(this->MetalStart == 1)
                    {
                        this->MetalStart = 2;
                    }
                    addBDFdata(BDF_LABLE_DATA,0,this->bdfData.EEG_BDF_data[BDF_LABLE_DATA]);
                }
            }
    }


}
