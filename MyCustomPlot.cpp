//#include "ui_MyMainWindow.h"
#include "MyCustomPlot.h"
#include <QMenu>
#include <QAction>
MyCustomPlot::MyCustomPlot(QWidget *parent) :
    QCustomPlot(parent),
    mZoomMode(true),
    mRubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{
    mRubberBand->setBackgroundRole(QPalette::Light);
    mRubberBand->setAutoFillBackground(true);

    setInteractions(QCP::iRangeDrag |QCP::iRangeZoom);
    //在上方、左方加入一层空白
    //plotLayout()->insertRow(0);
    //plotLayout()->insertColumn(0);
    plotLayout()->insertRow(0);

    m_lbxy  = new QLabel(this);
    QString LabelText = "X : "+QString::number(0,'f',2)+
            "   Y : "+QString::number(0,'f',2);
    m_lbxy->setText(LabelText);
    m_lbxy->setGeometry(20,40,250,15);   //标签显示
     //   QCPTextElement *title = new QCPTextElement(this, "", QFont("sans", 12, QFont::Bold));
    //    plotLayout()->addElement(0, 0, title);
    setInteraction(QCP::iRangeDrag,true);//使能拖动
    xAxis->setRange(0,4096);
    yAxis->setRange(0,100);
    m_Xaxis =  xAxis->range();
    m_graphIndex = 0;
    m_Yaxis =  yAxis->range();
    QCPAxisRect * axrect =new QCPAxisRect(this,false);
    axrect->setAutoMargins(QCP::msNone);
    axrect->insetLayout()->addElement(legend,Qt::AlignCenter);  //设置标签显示格式
    init();
}
MyCustomPlot::~MyCustomPlot()
{

}
void MyCustomPlot::setZoomMode(bool mode)
  {
    mZoomMode = mode;
  }

void MyCustomPlot::init()
{
    // add two new graphs and set their look:
    for (int graph_idx = 0; graph_idx< 10; graph_idx++)
    {
        addGraph();
        graph(graph_idx)->setPen(QPen(graph_idx * 256 * 256 * 256 / 10)); // 第0条曲线颜色
        graph(graph_idx)->setName("graph:" + QString::number(graph_idx));
    }
    //customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // 第0条曲线和0轴围成区域填充的颜色
    // generate some points of data (y0 for first, y1 for second graph):
//    QVector<double> x(251), y0(251), y1(251);
//    for (int i=0; i<251; ++i)
//    {
//      x[i] = i;
//      y0[i] = qExp(-i/150.0)*qCos(i/10.0); // exponentially decaying cosine
//      y1[i] = qExp(-i/150.0);              // exponential envelope
//    }
    // 配置右侧和顶部轴显示刻度，但不显示标签：
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    xAxis->setLabel("当前时间（时分秒）");
    xAxis2->setVisible(true);
    xAxis2->setTickLabels(false);
    yAxis2->setVisible(true);
    yAxis2->setTickLabels(false);

    //customPlot->axisRect()->setBackground(QBrush((QColor(100, 100, 100))));//背景黑色
    xAxis->grid()->setPen(QPen(QColor(180, 180, 180), 1, Qt::PenStyle::DashLine));//网格白色虚线
    yAxis->grid()->setPen(QPen(QColor(180, 180, 180), 1, Qt::PenStyle::DashLine));//网格白色虚线
    xAxis->grid()->setSubGridPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));//网格浅色点线
    yAxis->grid()->setSubGridPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));//网格浅色点线
    xAxis->grid()->setSubGridVisible(true);//显示x轴子网格线
    yAxis->grid()->setSubGridVisible(true);//显示要轴子网格线
    xAxis->grid()->setZeroLinePen(QPen(Qt::green));//x轴0线颜色白色
    yAxis->grid()->setZeroLinePen(QPen(Qt::green));//y轴0线颜色白色

    //使用时间作为横轴
    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    QDateTime dateTime = QDateTime::currentDateTime();
    //设置label 旋转35° 横着显示可能显示不全
    xAxis->setTickLabelRotation(35);
    timeTicker->setDateTimeFormat("yyyy/MM/dd-hh:mm:ss");//设置格式为17:22:49
    //customPlot->xAxis->setDateTimeSpec(Qt::LocalTime);
    //xAxis->setTicker(timeTicker);
    //四边安上坐标轴
    axisRect()->setupFullAxesBox();
    //设置y轴范围
    yAxis->setRange(-10,10);


    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), xAxis2, SLOT(setRange(QCPRange)));
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));

//    // pass data points to graphs:
////    customPlot->graph(0)->setData(x, y0);
////    customPlot->graph(1)->setData(x, y1);
//    //customPlot->graph(0)->addData(1,2);
//    // 让范围自行缩放，使图0完全适合于可见区域:
    graph(0)->rescaleAxes();
//    // 图1也是一样自动调整范围，但只是放大范围(如果图1小于图0)
//    customPlot->graph(1)->rescaleAxes(true);
//    // Note: we could have also just called customPlot->rescaleAxes(); instead
//    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    setInteractions(QCP::iRangeDrag  |QCP::iRangeZoom | QCP::iSelectPlottables);
    setSelectionRectMode(QCP::SelectionRectMode::srmZoom);//模式：框选放大
}
void MyCustomPlot::setPlotData(const QVector<float> plots, QString lineName)
{
    addGraph(); // blue line  //加线

    graph(m_graphIndex)->setPen(QPen(Qt::red));
    //customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    graph(m_graphIndex)->setAntialiasedFill(false);
    graph(m_graphIndex)->setName(lineName);
    int count = 0;
    for(auto it = plots.begin();it!=plots.end();it++)
    {
        graph(m_graphIndex)->addData(count, *it);
        count++;
    }
    m_graphIndex++;
    replot();
}
void MyCustomPlot::mousePressEvent(QMouseEvent * event)
{
//    if (mZoomMode) //判断是否可以触发
//    {
//        if (event->button() == Qt::RightButton)
//        {
//            mOrigin = event->pos();
//            mRubberBand->setGeometry(QRect(mOrigin, QSize()));
//            mRubberBand->show();
//        }
//    }
  QCustomPlot::mousePressEvent(event);
}

void MyCustomPlot::mouseMoveEvent(QMouseEvent * event)
{
//    int x_pos = event->pos().x();
//    int y_pos = event->pos().y();
//    mousePoint = event->pos();
//    //鼠标坐标转化为CustomPlot内部坐标
//    float x_val = xAxis->pixelToCoord(x_pos);
//    float y_val = yAxis->pixelToCoord(y_pos);
//    QString LabelText = "X : "+QString::number(x_val,'f',2)+
//            "   Y : "+QString::number(y_val,'f',2);
//    m_lbxy->setText(LabelText);
//     if (mRubberBand->isVisible())
//    {
//        mRubberBand->setGeometry(QRect(mOrigin, event->pos()).normalized());
//    }
    QCustomPlot::mouseMoveEvent(event);
}

void MyCustomPlot::wheelEvent(QWheelEvent *event)
{
//    qDebug()<<"wheelEvent:"<<event->delta();
//    int range_low = xAxis->range().lower;
//    int range_upp = xAxis->range().upper;
//    if (event->delta() > 0)//往上滚动,图像放大
//    {
//         range_low = -range_low/10;
//         range_upp = -range_upp/10;
////        //xAxis->scaleRange(1.5);
////        //xAxis->setRange(1.5);
////        qDebug() << xAxis->range();
////        xAxis->setRange(xAxis->range() / 1.5);
////        yAxis->setRange(yAxis->range());
////        qDebug() << xAxis->range();

//    }
//    else if (event->delta() < 0)//往下滚动，图像缩小
//    {
//        range_low = range_low/10;
//        range_upp = range_upp/10;
////        xAxis->setRange(100,4001.2);
////        xAxis->low
////        xAxis->setRange(xAxis->range() * 1.5);
////        yAxis->setRange(yAxis->range());
//    }

//    range_low = (range_low < 100) ? 100 : range_low;
//    range_upp = (range_upp < 100) ? 100 : range_upp;

//    xAxis->setRange(xAxis->range().lower + range_low,xAxis->range().upper + range_upp);
    //replot(QCustomPlot::rpQueuedReplot);
    QCustomPlot::wheelEvent(event);
}

//void MyCustomPlot::addTimeData(int idx,float value)
//{
//    float key = 0;//QTimer(Qt::Key_unknown).curr;
//    QCustomPlot::graph(idx)->addData(key,value);
//}
//void MyCustomPlot::addData(int idx,float key,float value)
//{
//    QCustomPlot::graph(idx)->addData(key,value);
//}

void MyCustomPlot::mouseReleaseEvent(QMouseEvent * event)
{
//    if (mRubberBand->isVisible())
//    {
//        const QRect zoomRect = mRubberBand->geometry();
//        int xp1, yp1, xp2, yp2;
//        if(zoomRect.width()>5&&zoomRect.height()>5)
//        {
//        //获取坐标
////            zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
////            double x1 = xAxis->pixelToCoord(xp1);
////            double x2 = xAxis->pixelToCoord(xp2);
////            double y1 = yAxis->pixelToCoord(yp1);
////            double y2 = yAxis->pixelToCoord(yp2);
//            qDebug() << mOrigin.x() << "," << mOrigin.y();
//            //开始x坐标比较小，说明是从左拉到右，进行放大
//            //if (mOrigin.x() <= xp1)
//            {
////                xAxis->setRange(x1, x2);
////                yAxis->setRange(y1, y2);
//            }
////            else
////            {
////                //从右往左滑动，将x显示的数据更多
////                double diff;
////                diff = x2 - x1;
////                xAxis->setRange(x1 - diff,x2 + diff);
////                //yAxis->setRange(yAxis->range()*2 + y1);
////            }
//            mRubberBand->hide();
//            replot();
//        }
//        else
//        {
//            QMenu *pMenu = new QMenu(this);

//            QAction *act_Clear = new QAction(tr("清空"), this);
//            QAction *act_Reset = new QAction(tr("复位"), this);
//            //把QAction对象添加到菜单上
//            pMenu->addAction(act_Clear);
//            pMenu->addAction(act_Reset);
//                    //连接鼠标右键点击信号
//            connect(act_Clear, SIGNAL(triggered()), this, SLOT(OnClearAction()));
//            connect(act_Reset, SIGNAL(triggered()), this, SLOT(OnResetAction()));

//            //在鼠标右键点击的地方显示菜单
//            pMenu->exec(cursor().pos());

//            //释放内存
//            QList<QAction*> list = pMenu->actions();
//            foreach (QAction* pAction, list) delete pAction;
//            delete pMenu;
//             }
//    }
//    mRubberBand->hide();
    QCustomPlot::mouseReleaseEvent(event);
}
//复位
void MyCustomPlot::OnResetAction()
{

    xAxis->setRange(m_Xaxis.lower,m_Xaxis.upper);
    //    xAxis->scaleRange(1.0,1.0);
    //    yAxis->scaleRange(1.0,1.0);
    yAxis->setRange(m_Yaxis.lower,m_Yaxis.upper);
    replot();
}
//清空
void MyCustomPlot::OnClearAction()
{
    m_graphIndex = 0;
    this->clearGraphs();

    m_graphIndex = 1;
    this->addGraph();

    replot();
}
