#include "ui_MyMainWindow.h"
#include "MyMainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tx_cnt = 0;
    rx_cnt = 0;
    serialPort = new QSerialPort(this);
    output_label = new QLabel(this);
    txrx_cnt_label = new QLabel(this);
    ymodem = new YMODEM(serialPort,this);
    dataProcess = new DataProcess();

    ui->statusBar->addWidget(output_label);
    ui->statusBar->addWidget(txrx_cnt_label);
    connect(serialPort,&QSerialPort::readyRead,this,&MainWindow::Serial_read);

    //on_opencom_pushButton_clicked();
    ui->tx_textBrowser->setText("0104 0000 0009");
    ui->rx_textBrowser->setText("此程序用于连接多功能传感器模块（S09A或S09B）以获取二氧化碳数据。");
    ui->rx_textBrowser->append("请点击“搜索串口”，然后设置波特率，再连接，勾选最下方的“定时发送”，便可看到二氧化碳曲线图。");
    //QValidator* validator = new QIntValidator( 88, 98, ui->lineEdit );    ui->lineEdit->setValidator( validator );

    //设置追踪鼠标，能响应鼠标事件
    //setMouseTracking(true);
    rx_flag = false;

    modbus = new Modbus();
    auto_send_timer = new QTimer(this);
    connect(auto_send_timer, &QTimer::timeout, this, &MainWindow::auto_send_timer_handle);

    //定时器连接槽函数realtimeDataSlot
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(16); // 间隔时间 0ms表示尽可能快的触发

    channel_show_checkBox_init();

}

MainWindow::~MainWindow()
{
    delete ui;

}


void MainWindow::on_com_Button_clicked()
{
   GetEnableCommPort();
}

void MainWindow::realtimeDataSlot()
{

    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds

    QCustomPlot* customPlot = ui->customPlot;
    int x_upper = ui->ycount_spinBox->value();

    //customPlot->xAxis->setRange(customPlot->graph(0)->dataCount(), x_upper, Qt::AlignRight);
    //重绘
    customPlot->replot(QCustomPlot::rpQueuedReplot);

    if (ui->refresh_checkBox->checkState() == Qt::Checked)
    {
        ui->customPlot->graph(0)->rescaleAxes();
    }
}
void MainWindow::redraw(float* y_value,int data_cnt)
{
    static double x_value = 0;
//    if (ui->realtime_radioBtn->isChecked())
//    {
//        x_value = QDateTime::currentSecsSinceEpoch(); // 开始到现在的时间，单位秒
//    }
//    else
    {
        x_value += 1;
    }
    for (int data_idx = 0; data_idx < data_cnt; data_idx++)
    {
        float k = ui->ch00_k_Edit->text().toFloat();
        float b = ui->ch00_b_Edit->text().toFloat();

        *(y_value + data_idx) = *(y_value + data_idx) * k + b;
        qDebug()<<"data"<<  *(y_value + data_idx);
        ui->customPlot->graph(data_idx)->addData(x_value,*(y_value + data_idx));
    }
    //ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
}

QVector<double> y_value[4],x_value;

void qt_read_csv_test()
{
    static int i = 0;
    i /= 4;
    QFile inFile("2.csv");
    QStringList lines;/*行数据*/
    if (inFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream_text(&inFile);
        while (!stream_text.atEnd())
        {
            lines.push_back(stream_text.readLine());
        }
        for (int j = 1; j < lines.size(); j++)
        {
            QString line = lines.at(j);
            QStringList split = line.split(",");/*列数据*/
            for (int col = 0; col < split.size(); col++)
            {
                i++;
                QString str = split.at(col);
                y_value[j-1].push_back(str.toFloat());
                if (j == 1)
                    x_value.push_back(i);
                qDebug() << str << " ";
            }
            //cout << endl;
        }
        inFile.close();
    }
}

void MainWindow::Serial_read()
{
    if (ui->rx_show_checkBox->checkState() == Qt::Unchecked)
    {
        return;
    }

    //读取串口收到的数据
    //QThread::msleep(1);
    rx_buf = serialPort->readAll();

    // 获取数据头指针，判断数据完整性
    float *data_ptr = dataProcess->minibalance(rx_buf);
    if (nullptr != data_ptr)
    {
        // 获取解析成功的数据的个数
        int data_cnt = dataProcess->count();

        // 进行绘图
        redraw(data_ptr,data_cnt);
    }

    //回环模式
    if (ui->loop_checkBox->checkState() == Qt::Checked)
    {
        if (serialPort->isOpen())
        {
            serialPort->write(rx_buf);
        }
    }

    rx_cnt += rx_buf.length();
    // 判断是否需要16进制显示
    if(ui->rxhex_checkBox->isChecked()==true)
    {
        rx_buf = rx_buf.toHex('\0') ;//转换为16进制 例："1234" -->“31323334”
    }

    // 在接收窗口显示收到的数据
    ui->rx_textBrowser->append(QString(rx_buf));

    txrx_cnt_label->setText("tx:" + QString::number(tx_cnt) + ",rx" + QString::number(rx_cnt));
    rx_buf.clear();
}


// 获取计算机可用串口
QStringList MainWindow::GetEnableCommPort()
{
    QStringList CommPortList;
    ui->com_comboBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        ui->com_comboBox->addItem (serial.portName());
    }

//    static int i = 0;
//    int x = i + 10000;
//    for ( ;i<x;i++)
//    {
//        redraw(sin (i/100.0f) * i * 1.0f);
//    }
    return CommPortList;
}

void MainWindow::on_opencom_pushButton_clicked()
{
    unsigned int baudrate = ui->baud_comboBox->currentText().toInt();
    enum QSerialPort::Parity parity = QSerialPort::NoParity;
    enum QSerialPort::DataBits databits = QSerialPort::Data8;
    if(ui->com_comboBox->currentText() == NULL)
    {
        GetEnableCommPort();
    }
    if (baudrate == 0)
    {
        ui->baud_comboBox->setEditText("0");
        baudrate = ui->baud_comboBox->currentText().toInt();

    }

    databits = (enum QSerialPort::DataBits)ui->databits_comboBox->currentText().toInt();

    if (ui->paritybit_comboBox->currentText() == "偶校验")
    {
        parity = QSerialPort::EvenParity;
    }
    else if (ui->paritybit_comboBox->currentText() == "奇校验")
    {
        parity = QSerialPort::OddParity;
    }
    serialPort->setPortName(ui->com_comboBox->currentText());
    serialPort->setBaudRate(baudrate);
    serialPort->setDataBits(databits);
    serialPort->setParity(parity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    qDebug()<<ui->databits_comboBox->currentData();
    if (serialPort->isOpen())
    {
        //因为更新上方配置可能导致需要重新打开串口
        qDebug()<<"串口已关闭";
        ui->autosend_checkBox->setCheckState(Qt::Unchecked);
        ui->opencom_pushButton->setText("串口已关闭");
        serialPort->close();
    }
    else
    {
        if( !serialPort->open(QIODevice::ReadWrite) )
        {
            ui->opencom_pushButton->setText("串口打开失败,请稍后重试!");
        }
        else
        {
            ui->opencom_pushButton->setText("串口打开成功");
        }
    }
}

unsigned short CRC16 ( QByteArray arr_buff, unsigned char len)
{

    int i,pos;
      unsigned int crc = 0xFFFF;
      for ( pos = 0; pos < len; pos++)
      {
          crc ^= (unsigned char)arr_buff[pos];
          for (i = 8; i != 0; i--)   // Loop over each bit
          {
              if ((crc & 0x0001) != 0)   // If the LSB is set
              {
                  crc >>= 1; // Shift right and XOR 0xA001
                  crc ^= 0xA001;
              }
              else // Else LSB is not set
              {
                  crc >>= 1;    // Just shift right
              }
          }
      }
      //高低字节转换
      //crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
      return crc;
}
void MainWindow::on_send_pushButton_clicked()
{
    QByteArray tx_data = ui->tx_textBrowser->toPlainText().toLatin1();

    if (ui->txhex_checkBox->checkState() == Qt::Checked)
    {
        tx_data = QByteArray::fromHex(ui->tx_textBrowser->toPlainText().toLatin1()); //转换为16进制
    }
    if (ui->crc16modbus_checkBox->checkState() == Qt::Checked)
    {
        unsigned short crc =  CRC16(tx_data,tx_data.length());
        tx_data += crc & 0xff;
        tx_data += crc >> 8;
        output_label->setText("附加发送数据为：0x" + QString::number(crc & 0xff,16)
                              + " 0x" + QString::number(crc >> 8,16));
    }
    if (serialPort->isOpen())
    {
        serialPort->write(tx_data);
    }
    tx_cnt += tx_data.length();
    txrx_cnt_label->setText("tx:" + QString::number(tx_cnt) + ",rx" + QString::number(rx_cnt));
}

void MainWindow::on_clearrx_pushButton_clicked()
{
    ui->rx_textBrowser->clear();
    rx_cnt = 0;
    output_label->setText("接收区已清空");
}


void MainWindow::on_update_pushButton_clicked()
{
    ui->customPlot->graph(0)->rescaleAxes(true);
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);
    //ui->rx_show_checkBox->setCheckState(Qt::Unchecked);
    //frmupgrade1 = new frmUpgrade(serialPort);
    //frmupgrade1->show();
}


//01 03 12 (12个xx)  crc_l crc_h
void MainWindow::auto_send_timer_handle()
{
    on_send_pushButton_clicked();
}

//查找modbus id
void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setDisabled(true);
    qDebug()<<"你按下了按键";
    ui->pushButton->setDisabled(false);

}

void MainWindow::on_autosend_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Unchecked)
    {
        auto_send_timer->stop();
        ui->autosend_lineEdit->setEnabled(true);
    }
    else if (arg1 == Qt::Checked)
    {
        ui->autosend_lineEdit->setDisabled(true);
        int send_mdelay = ui->autosend_lineEdit->text().toUInt();
        if (send_mdelay == 0)
        {
            ui->autosend_lineEdit->setText("3000");
            send_mdelay = 3000;
        }
        auto_send_timer->start(send_mdelay);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    qt_read_csv_test();
    x_value.pop_front();
    for (int i = 0;i<4 ;i++)
    {
        ui->customPlot->graph(i)->setName(QString::number(y_value[i].at(0)));
        y_value[i].pop_front();
        ui->customPlot->graph(i)->setData(x_value,y_value[i]);
    }
}

void MainWindow::on_refresh_checkBox_stateChanged(int)
{
    ui->customPlot->graph(0)->rescaleAxes();
    for (int i = 0; i< ui->customPlot->graphCount();i++)
    {
        ui->customPlot->graph(i)->rescaleAxes(true);
    }
    QSharedPointer<QCPGraphDataContainer> s = ui->customPlot->graph(0)->data();
    ui->customPlot->replot(QCustomPlot::rpQueuedReplot);

}

void MainWindow::on_keeptop_checkBox_stateChanged(int arg1)
{
    Qt::WindowFlags m_flags = windowFlags();
    if (arg1 == Qt::Checked)
    {
        m_flags |= Qt::WindowStaysOnTopHint;
    }
    else
    {
        m_flags &= ~Qt::WindowStaysOnTopHint;
    }
    setWindowFlags(m_flags);

    show();
}

void MainWindow::on_cleanwave_Btn_clicked()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->init();
}
void MainWindow::channel_show_checkBox_init()
{
    //QLineEdit *k_edit = new QLineEdit(ui->groupBox);
    //k_edit

    connect(ui->ch00_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch01_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch02_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch03_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch04_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch05_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch06_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch07_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch08_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
    connect(ui->ch09_show_checkBox,&QCheckBox::stateChanged,this,&MainWindow::channel_show_checkBox_stateChanged);
}

void MainWindow::channel_show_checkBox_stateChanged(int arg1)
{

    int checkbox_idx = 0;
    QCheckBox* checkbox = (QCheckBox*)sender();
         if (checkbox == ui->ch00_show_checkBox) checkbox_idx = 0;
    else if (checkbox == ui->ch01_show_checkBox) checkbox_idx = 1;
    else if (checkbox == ui->ch02_show_checkBox) checkbox_idx = 2;
    else if (checkbox == ui->ch03_show_checkBox) checkbox_idx = 3;
    else if (checkbox == ui->ch04_show_checkBox) checkbox_idx = 4;
    else if (checkbox == ui->ch05_show_checkBox) checkbox_idx = 5;
    else if (checkbox == ui->ch06_show_checkBox) checkbox_idx = 6;
    else if (checkbox == ui->ch07_show_checkBox) checkbox_idx = 7;
    else if (checkbox == ui->ch08_show_checkBox) checkbox_idx = 8;
    else if (checkbox == ui->ch09_show_checkBox) checkbox_idx = 9;

    ui->customPlot->graph(checkbox_idx)->setVisible(Qt::Checked == arg1);
}
