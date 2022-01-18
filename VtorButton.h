#ifndef VTORBUTTON_H
#define VTORBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QEnterEvent>

class VtorButton : public QPushButton
{
public:
    VtorButton(QWidget *parent = nullptr);
    ~VtorButton();

    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

#endif // VtorBUTTON_H
