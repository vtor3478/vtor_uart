#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QLabel>
#include <ymodem.h>
#include <QPaintEvent>
#include <QPainter>
#include "frmupgrade.h"
#include "MyCustomPlot.h"
#include "Modbus.h"
#include "VtorButton.h"
#include "dataprocess.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTimer dataTimer;

    void channel_show_checkBox_init(void);

private slots:
    void on_com_Button_clicked();

    void on_opencom_pushButton_clicked();

    void on_send_pushButton_clicked();

    void Serial_read();

    void on_clearrx_pushButton_clicked();

    void on_update_pushButton_clicked();

    //void QPaintEvent(QEvent *e);
    void auto_send_timer_handle(void);  //使用定时器进行定时发送

    void on_pushButton_clicked();

    void redraw(float *y_value,int data_cnt = 1);

    void realtimeDataSlot();

    //qint64 serialPort_write(QByteArray data);


    void on_autosend_checkBox_stateChanged(int arg1);

    void on_pushButton_2_clicked();

    void on_refresh_checkBox_stateChanged(int arg1);

    void on_keeptop_checkBox_stateChanged(int arg1);

    void on_cleanwave_Btn_clicked();

    void channel_show_checkBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    //QSerialPort *serialPort;
    QStringList GetEnableCommPort();
    QLabel *output_label;
    QLabel *txrx_cnt_label;
    YMODEM *ymodem;
    frmUpgrade *frmupgrade1;
    QTimer *auto_send_timer;
    int tx_cnt;
    int rx_cnt;
    bool rx_flag;
    QByteArray rx_buf;
    QByteArray tx_buf;
    Modbus* modbus;
    DataProcess *dataProcess;
};

#endif // MYMAINWINDOW_H
