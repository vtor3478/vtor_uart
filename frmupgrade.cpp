#include "frmupgrade.h"
#include "ui_frmupgrade.h"
#include <QFileDialog>
#include <QDialog>
#include <QMessageBox>
#include "delay.h"

frmUpgrade::frmUpgrade(QSerialPort* serial, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmUpgrade),
    m_serial(serial)
{
    ui->setupUi(this);

    update_ba.append (0xFE);
    update_ba.append (0x69);
    update_ba.append (0x81);
    update_ba.append (0xFE);

    cmd_suc_ba.append(0xFE);
    cmd_suc_ba.append(0x69);
    cmd_suc_ba.append(0x01);
    cmd_suc_ba.append((char)0);
    cmd_suc_ba.append(0xE0);
    cmd_suc_ba.append(0x40);
}

frmUpgrade::~frmUpgrade()
{
    delete ui;
}

void frmUpgrade::on_select_pushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("打开升级文件"),tr("d:"),tr("Bin(*.bin)"));
    if( filePath.length () > 0 )
    {
      ui->filePathEdit->setText (filePath);
    }
}

void frmUpgrade::on_updateBtn_clicked()
{
    ui->updateBtn->setDisabled (true);
    if( !ui->filePathEdit->toPlainText().isEmpty () )
    {
      ui->updateProgressBar->setValue (0);

      QByteArray rec_data;
      QByteArray send;
      int retry_cnt = 0;
      int max_retry_cnt = 10;
      send.append ('1');
      m_serial->write (send,send.count ());
      delay_ms (100);
      rec_data = m_serial->read (cmd_suc_ba.count ());
      m_serial->clear ();

      if( rec_data.isEmpty () )
      {
        delay_ms (3000);
        m_serial->write (update_ba,update_ba.count ());
        while( !m_serial->waitForReadyRead (10))
        {
          ++retry_cnt;
          if( retry_cnt >= max_retry_cnt )
          {
            break;
          }
        }
        retry_cnt = 0;
        delay_ms (50);
        rec_data = m_serial->read (cmd_suc_ba.count ());
        m_serial->clear ();
        if( rec_data != cmd_suc_ba )
        {
          QMessageBox::information (this,"提示","写入升级命令失败，请稍后重试!",QMessageBox::Ok);
          ui->updateBtn->setDisabled (false);
          return;
        }

        while( !m_serial->waitForReadyRead (500))
        {
          ++retry_cnt;
          if( retry_cnt >= max_retry_cnt )
          {
            break;
          }
        }
        retry_cnt = 0;
        delay_ms (1000);
        rec_data = m_serial->read (10);
        m_serial->clear ();
        if( !(rec_data.contains ("======")) )
        {
          QMessageBox::information (this,"提示","进入IAP失败,请重试!",QMessageBox::Ok);
          ui->updateBtn->setDisabled (false);
          return;
        }

        delay_ms (500);
        m_serial->write (send,send.count ());
        while( !m_serial->waitForReadyRead (10))
        {
          ++retry_cnt;
          if( retry_cnt >= max_retry_cnt )
          {
            break;
          }
        }
        retry_cnt = 0;
        rec_data = m_serial->read (7);
        if( !(rec_data.contains ("Waiting")) )
        {
          QMessageBox::information (this,"提示","进入下载过程失败,请重试!",QMessageBox::Ok);
          ui->updateBtn->setDisabled (false);
          return;
        }
      }

      delay_ms (500);
      m_serial->clear ();
      YMODEM* update = new YMODEM(m_serial);
      connect (update, SIGNAL(packet(int,int)),this,SLOT(setProgressBarMinMax(int,int)));
      connect (update, SIGNAL(progress(int)), this, SLOT(setprogressBarVal(int)));
      if(update->yTransmit (ui->filePathEdit->toPlainText()) == 0 )
      {
        update->yTransmitClose ();
        QMessageBox::information (this,"提示","更新程序成功!",QMessageBox::Ok);
      }
      else
      {
        update->yTransmitClose ();
        QMessageBox::information (this,"提示","更新程序失败,请重试!",QMessageBox::Ok);
      }
      delete update;

      m_serial->clear ();
      rec_data.clear ();
    }
    else
    {
      QMessageBox::information (this,"提示","请先选择文件!",QMessageBox::Ok);
    }

    ui->updateBtn->setDisabled (false);
}
