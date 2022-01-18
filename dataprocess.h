#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <iostream>
#include <QObject>

class DataProcess
{
public:
    DataProcess();

    float *minibalance(QByteArray bytearray);
    int count();

private:
    float *char_point;
    float *short_point;
    float *int_point;
    float *float_point;
    int data_cnt;
    // 接收到的原始数据
    QByteArray rawdata;
};

#endif // DATAPROCESS_H
