#include "Delay.h"
#include <QCoreApplication>

void delay_ms(unsigned int mSec)
{
    QTime dieTime = QTime::currentTime().addMSecs(mSec);

    while ( QTime::currentTime() < dieTime )
    {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}
