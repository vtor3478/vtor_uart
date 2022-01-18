#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include <QObject>
#include "qcustomplot.h"

class MyCustomPlot : public QCustomPlot
{
    Q_OBJECT
    public:
    explicit MyCustomPlot(QWidget *parent = 0);
    ~MyCustomPlot();
    void setZoomMode(bool mode);
    void init();
public slots:
    void setPlotData(const QVector<float> plots,QString lineName);
protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent *event);
protected slots:
    void OnResetAction();
    void OnClearAction();
private:
    int m_graphIndex;
    bool mZoomMode;
    bool m_isMove;
    QRubberBand * mRubberBand;
    double m_minX,m_maxX,m_minY,m_maxY;
    QCPRange m_Xaxis,m_Yaxis;
    QPoint mousePoint;
    QPoint mOrigin;
    QLabel *m_lbxy;
};

#endif // MyCUSTOMPLOT_H
