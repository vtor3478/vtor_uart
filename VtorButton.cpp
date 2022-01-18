#include "VtorButton.h"

VtorButton::VtorButton(QWidget *parent)
    :QPushButton(parent)
{

}
VtorButton::~VtorButton()
{

}

void VtorButton::enterEvent(QEvent *e)
{
    this->resize(this->width() * 2,height() * 2);
    return QPushButton::enterEvent(e);
}

void VtorButton::leaveEvent(QEvent *e)
{
    this->resize(width() / 2,height() / 2);
    return QPushButton::enterEvent(e);
}
