#include "dataprocess.h"
#include <QDebug>

DataProcess::DataProcess()
{
    float_point = new float[20];
}

int DataProcess::count()
{
    return data_cnt;
}
float* DataProcess::minibalance(QByteArray received_data)
{
    int data_idx = 0;
    rawdata += received_data;
    static bool start_flag = false;
    if (rawdata.length() == 0)
    {
        return  nullptr;
    }
    // 解析数据还未开始，先寻找帧头,
    //if (false == start_flag)
    {
        int frame_start = rawdata.lastIndexOf('$');
        if (frame_start >= 0)
        {
            start_flag = true;
            // 获取从帧头获取的数据
            if (frame_start != 0)
            {
                rawdata = rawdata.right(rawdata.length() - frame_start);
            }
        }
    }
    //else
    {
        // 已经找到帧头，验证数据完整性
        for (data_idx = 1; 1 + data_idx * 4 < rawdata.length(); data_idx++)
        {
            qDebug() << 1 + data_idx * 4;// + ":" + rawdata.at(1 + data_idx * 4);
            qDebug() << rawdata.at(1 + data_idx * 4);
            if ( 1 + data_idx * 4 == rawdata.at(1 + data_idx * 4))
            {
                data_cnt = data_idx;
                start_flag = false;
                break;
                // 协议匹配，解析完成，一帧数据结束
            }
        }

        // 解析完成，start_flag才会变成false，才认为解析成功
        if (false == start_flag)
        {
            float value;
            char* p = (char*)&value;
            qDebug()<<"data_cnt:"<<data_cnt;
            for (int ch_idx = 0; ch_idx < data_cnt; ch_idx ++)
            {
                *(p + 0) = rawdata.at(ch_idx * 4 + 1);
                *(p + 1) = rawdata.at(ch_idx * 4 + 2);
                *(p + 2) = rawdata.at(ch_idx * 4 + 3);
                *(p + 3) = rawdata.at(ch_idx * 4 + 4);
                *(float_point + ch_idx) = value;
                qDebug() << *(float_point + ch_idx);
            }
            rawdata.clear();
            return float_point;
        }
    }
    //
    return nullptr;
}
