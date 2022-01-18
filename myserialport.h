#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H
#include <QWidget>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>


class MySerialPort : public QSerialPort
{
    Q_OBJECT
public:
    MySerialPort(QObject *parent = nullptr);
};

#endif // MYSERIALPORT_H
