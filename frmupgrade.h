#ifndef FRMUPGRADE_H
#define FRMUPGRADE_H

#include <QWidget>
#include <QtSerialPort>
#include <ymodem.h>

namespace Ui {
class frmUpgrade;
}

class frmUpgrade : public QWidget
{
    Q_OBJECT

public:
    explicit frmUpgrade(QSerialPort* serial, QWidget *parent = nullptr);
    ~frmUpgrade();

private slots:
    void on_select_pushButton_clicked();

    void on_updateBtn_clicked();

private:
    Ui::frmUpgrade *ui;
    QSerialPort* m_serial;
    QByteArray update_ba;
    QByteArray cmd_suc_ba;
};

#endif // FRMUPGRADE_H
