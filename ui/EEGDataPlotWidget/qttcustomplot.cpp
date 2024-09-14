#include "qttcustomplot.h"
#include <QDebug>

/*
 *
 *
 *
 *
 */

QttCustomPlot::QttCustomPlot(int count) : QCustomPlot(), rCount(count), AxisHide(false), legendVisible(false), isAutoScroll(false)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    init_customplot();
}

void QttCustomPlot::init_customplot()
{
    plotLayout()->clear();   // 清空原布局及其中部件

    this->setInteractions(QCP::iRangeDrag //可平移
                          | QCP::iRangeZoom); //可滚轮缩放
//                          | QCP::iSelectPlottables //可选中曲线
//                          | QCP::iSelectLegend );//可选中图例

//    this->setNoAntialiasingOnDrag(true);//禁用抗锯齿，以提高性能

    plotLayout()->setRowSpacing(0);
    plotLayout()->setColumnSpacing(0);
    plotLayout()->setMargins(QMargins(0,0,0,0));

    init_LayoutGrid(); //布局

    init_Rect(); //初始化坐标轴

    init_legend(); //初始化legend
//    toolTip = new QCPToolTip(this);


}

//!
void QttCustomPlot::init_LayoutGrid()
{
    plotLayoutGrid = new QCPLayoutGrid;
    legendLayoutGrid = new QCPLayoutGrid;

    plotLayout()->addElement(0, 0, plotLayoutGrid);
    plotLayout()->addElement(0, 1, legendLayoutGrid);
    plotLayout()->setColumnStretchFactor(1,0.001);

    plotLayoutGrid->setMargins(QMargins(0, 0, 0, 0));
    plotLayoutGrid->setRowSpacing(0);
    plotLayoutGrid->setColumnSpacing(0);
}

//初始化和添加新的横坐标分开 初始化ID直接是0就可以
//!
void QttCustomPlot::init_Rect()
{

    for (int i = 0; i < rCount; ++i) {

        QCPAxisRect* Rect = new QCPAxisRect(this, true);
        QCPAxis *xAxis = Rect->axis(QCPAxis::atBottom);
        QCPAxis *yAxis = Rect->axis(QCPAxis::atLeft);

        xAxis->setRange(StartX, 5);
        Rect->setupFullAxesBox(true);    // 让四个轴关联，并全部显示出来
      //  Rect->setRangeDrag(Qt::Horizontal | Qt::Vertical); //水平方向拖动

      //  Rect->setRangeZoom(Qt::Horizontal | Qt::Vertical); //水平方向缩放

//        Rect->axis(QCPAxis::atLeft)->setLabel(QString::number(0));


        //横坐标属性设置
        QFont font = QFont(AppFontName, AppFontSize - 2);
        xAxis->setLabelFont(font);
        yAxis->setLabelFont(font);
        xAxis->setTickLabelFont(font);
        yAxis->setTickLabelFont(font);
        yAxis->setLabelColor(TextColor);
        yAxis->setTickLabelColor(TextColor);
        yAxis->setBasePen(QPen(TextColor, TextWidth));
        yAxis->setTickPen(QPen(TextColor, TextWidth));
        yAxis->setSubTickPen(QPen(TextColor, TextWidth));
        //设置坐标颜色/坐标名称颜色
        xAxis->setTickLabelColor(TextColor);

        xAxis->setBasePen(QPen(TextColor, TextWidth));

        xAxis->setTickPen(QPen(TextColor, TextWidth));

        xAxis->setSubTickPen(QPen(TextColor, TextWidth));

        xAxis->setVisible(true);
//        xAxis->setTickLabels(false);
//        xAxis->grid()->setSubGridVisible(false);
//        xAxis->setTicks(false);
        yAxis->setVisible(false);
        yAxis->setTickLabels(false);
        xAxis->grid()->setSubGridVisible(false);


        plotLayoutGrid->addElement(i, 0, Rect);

        RectList.append(Rect);

    }
}

//!
//! \brief QttCustomPlot::setLegendVisible
//! \param on
//!
void QttCustomPlot::init_legend()
{
    legend = new QCPLegend;
    legendLayoutGrid->addElement(legend);
    setLegendVisible(legendVisible); //隐藏legend，坐标系中没有图层时，坐标系会缩小，是由于右侧legend没有item时，会最小化，导致布局中，左侧也跟着最小化，解决方法，隐藏右侧布局（实际上是移除）或者使右侧布局保持最大化（未做到）
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(when_selectionChangedByUser()));
    connect(this, SIGNAL(legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)),
            this, SLOT(when_legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)));
}

void QttCustomPlot::setLegendVisible(bool on)
{
    legendVisible = on;
    if(on &&/* (graphCount > 0)*/legend->itemCount() > 0 && !plotLayout()->hasElement(0,1))
    {
        legendLayoutGrid->setVisible(on);
        plotLayout()->addElement(0, 1, legendLayoutGrid);
        plotLayout()->setColumnStretchFactor(1,0.001);
        if(!legendLayoutGrid->hasElement(0,0))
            legendLayoutGrid->addElement(legend);
    }
    else if(!on)
    {
        if(plotLayout()->hasElement(0,1))
            plotLayout()->take(legendLayoutGrid);
        legendLayoutGrid->setVisible(on);
    }
    replot(QCustomPlot::rpQueuedReplot);
}

//!
void QttCustomPlot::addRect(int idx)
{
//        QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);       //以智能指针方式定义dateTicker
//        dateTicker->setDateTimeFormat("hh:mm:ss");
//        dateTicker->setTickCount(10);
//        dateTicker->setTickStepStrategy(QCPAxisTicker::tssReadability);
//       Rect->axis(QCPAxis::atBottom)->setTicker(dateTicker);

    QCPAxisRect* Rect = new QCPAxisRect(this, true);
    QCPAxis *xAxis = Rect->axis(QCPAxis::atBottom);
    QCPAxis *yAxis = Rect->axis(QCPAxis::atLeft);


    xAxis->setRange(StartX, 5);

    Rect->setupFullAxesBox(true);    // 让四个轴关联，并全部显示出来

//    Rect->setRangeDrag(Qt::Horizontal | Qt::Vertical); //水平方向拖动

//    Rect->setRangeZoom(Qt::Horizontal | Qt::Vertical); //水平方向缩放

    Rect->axis(QCPAxis::atLeft)->setLabel(QString::number(idx));

    //横坐标属性设置
    QFont font = QFont(AppFontName, AppFontSize - 2);
    xAxis->setLabelFont(font);
    yAxis->setLabelFont(font);
    xAxis->setTickLabelFont(font);
    yAxis->setTickLabelFont(font);
    yAxis->setLabelColor(TextColor);
    yAxis->setTickLabelColor(TextColor);
    yAxis->setBasePen(QPen(TextColor, TextWidth));
    yAxis->setTickPen(QPen(TextColor, TextWidth));
    yAxis->setSubTickPen(QPen(TextColor, TextWidth));
    //设置坐标颜色/坐标名称颜色
    xAxis->setTickLabelColor(TextColor);

    xAxis->setBasePen(QPen(TextColor, TextWidth));

    xAxis->setTickPen(QPen(TextColor, TextWidth));

    xAxis->setSubTickPen(QPen(TextColor, TextWidth));


    xAxis->setVisible(false);
    xAxis->setTickLabels(false);
    xAxis->grid()->setSubGridVisible(false);
    xAxis->setTicks(false);
    yAxis->setVisible(false);
    yAxis->setTickLabels(false);
    xAxis->grid()->setSubGridVisible(false);



    plotLayoutGrid->insertRow(idx);
    plotLayoutGrid->addElement(idx, 0, Rect);
    plotLayoutGrid->simplify();

    RectList.insert(idx,Rect);
    yAxis->setTicks(false);
    hideAxises(1);

//    Rect->axis(QCPAxis::atLeft)->setVisible(false);
//    Rect->axis(QCPAxis::atBottom)->setVisible(false);
//    Rect->axis(QCPAxis::atRight)->setVisible(false);
//    Rect->axis(QCPAxis::atTop)->setVisible(false);

    rCount++;

    qreplot();
}

//!
void QttCustomPlot::removeRect(int idx)
{
    if(idx >= RectList.length()) return;
    auto Rect = RectList.at(idx);
    if(Rect!= nullptr)
    {
        RectList.removeAt(idx);
        plotLayoutGrid->remove(Rect);
        plotLayoutGrid->simplify();
        Rect = nullptr;

        rCount--;

        qreplot();
    }
}

//!
void QttCustomPlot::hideAxis(int rectId, QCPAxis::AxisType type, bool on)
{
    if(rectId >= RectList.length() || rectId < 0)
    {
        qDebug() << Q_FUNC_INFO << "RectList index out of bounds:" << rectId;
        return;
    }

    auto Rect = RectList.at(rectId);
    Rect->axis(type)->setVisible(!on);
    qreplot();
}

//!
void QttCustomPlot::hideAxises(bool on)
{
    AxisHide = on;

    int count = RectList.length();
    for (int i = 0; i < count; ++i) {
        auto Rect = RectList.at(i);
        if(i == 0)
        {
            Rect->axis(QCPAxis::atBottom)->setVisible(!on);
            Rect->axis(QCPAxis::atRight)->setVisible(!on);
            Rect->axis(QCPAxis::atTop)->setVisible(!on);


//            Rect->axis(QCPAxis::atBottom)->setVisible(on);
//             Rect->axis(QCPAxis::atBottom)->setTickLabels(on);
//             QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);       //以智能指针方式定义dateTicker

//             dateTicker->setDateTimeFormat("hh:mm:ss");
//             dateTicker->setTickCount(10);
//             dateTicker->setTickStepStrategy(QCPAxisTicker::tssReadability);
//             Rect->axis(QCPAxis::atBottom)->setTicker(dateTicker);

        }
        else if(i == count -1)
        {
            Rect->axis(QCPAxis::atTop)->setVisible(!on);
            Rect->axis(QCPAxis::atRight)->setVisible(!on);
            Rect->axis(QCPAxis::atBottom)->setVisible(on);
            Rect->axis(QCPAxis::atBottom)->setTicks(on);
            Rect->axis(QCPAxis::atBottom)->setTickLabels(on);


            QPen pen;
            pen.setColor(QColor(153, 153, 153));//主刻度颜色
            pen.setWidth(2);//线宽2
            Rect->axis(QCPAxis::atBottom)->setTickPen(pen);
             Rect->axis(QCPAxis::atBottom)->setTickLengthIn(1000);
            //             QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);       //以智能指针方式定义dateTicker

//                         dateTicker->setDateTimeFormat("hh:mm:ss");
//                         dateTicker->setTickCount(10);
//                         dateTicker->setTickStepStrategy(QCPAxisTicker::tssReadability);
//                         Rect->axis(QCPAxis::atBottom)->setTicker(dateTicker);


        }
        else
        {
            Rect->axis(QCPAxis::atTop)->setVisible(!on);
            Rect->axis(QCPAxis::atBottom)->setVisible(!on);
            Rect->axis(QCPAxis::atRight)->setVisible(!on);



        }
    }

    connectAllxAsix(on);

    qreplot();
}


//!
QCPGraph* QttCustomPlot::getGraph(int RectId, int GraphId)
{
    QCPGraph* graph = nullptr;
    if(RectId < RectList.length() && RectId > 0)
    {
        auto list = RectList.at(RectId)->graphs();
        if(GraphId > 0 && GraphId < list.length())
        {
            graph = list.at(GraphId);
        }
    }
    return graph;
}

void QttCustomPlot::setLegen(int RectId, int GraphId,QString str)
{
    if(RectId < 0 || RectId >= RectList.length())
    {
        qDebug() << Q_FUNC_INFO << "RectList index out of bounds:" << RectId;
        return;
    }

    auto rect = RectList.at(RectId);

    QCPGraph* _graph = nullptr;

    if(GraphId < 0)
    {
         qDebug()<< Q_FUNC_INFO << "rect.graphs() index cannot be attached:" << GraphId;
         return;
    }
    else if(GraphId < rect->graphs().length())
    {
        _graph = rect->graphs().at(GraphId);
    }
    else if(GraphId == rect->graphs().length())
    {
        _graph = addGraph(rect->axis(QCPAxis::atBottom),rect->axis(QCPAxis::atLeft));

        setGraphColor(_graph,RectId+1);
    }
    else
    {
        qDebug()<< Q_FUNC_INFO << "GraphId值过大:" << GraphId << "应为："<<rect->graphs().length();
        return;
    }


  //  RectList.at(RectId)->graphs().at(GraphId)->setName(str);

    _graph->setName(str);
//    My_legend= new QCPLegend;
//    legend->addItem(_graph->name(), _graph->lineStyle());







    qreplot();
}


//!
void QttCustomPlot::addData(int RectId, int GraphId, double x, double y)
{
    if(RectId < 0 || RectId >= RectList.length())
    {
        qDebug() << Q_FUNC_INFO << "RectList index out of bounds:" << RectId;
        return;
    }

    auto rect = RectList.at(RectId);

    QCPGraph* _graph = nullptr;

    if(GraphId < 0)
    {
         qDebug()<< Q_FUNC_INFO << "rect.graphs() index cannot be attached:" << GraphId;
         return;
    }
    else if(GraphId < rect->graphs().length())
    {
        _graph = rect->graphs().at(GraphId);
    }
    else if(GraphId == rect->graphs().length())
    {
        _graph = addGraph(rect->axis(QCPAxis::atBottom),rect->axis(QCPAxis::atLeft));

//        rect->axis(QCPAxis::atBottom)->setRange(qAbs(x)-10,x);
//        rect->axis(QCPAxis::atLeft)->setRange(qAbs(y)-1,y);

        setGraphColor(_graph,RectId+1);
    }
    else
    {
        qDebug()<< Q_FUNC_INFO << "GraphId值过大:" << GraphId << "应为："<<rect->graphs().length();
        return;
    }

    _graph->addData(x, y);

   // _graph->rescaleAxes(true);
//    autoScroll();

   // qreplot();
}

//!
void QttCustomPlot::addData(int RectId, int GraphId, QVector<double> x, QVector<double> y)
{
    if(RectId < 0 || RectId >= RectList.length())
    {
        qDebug() << Q_FUNC_INFO << "RectList index out of bounds:" << RectId;
        return;
    }

    auto rect = RectList.at(RectId);

    QCPGraph* _graph = nullptr;

    if(GraphId < 0)
    {
         qDebug()<< Q_FUNC_INFO << "rect.graphs() index cannot be attached:" << GraphId;
         return;
    }
    else if(GraphId < rect->graphs().length())
    {
        _graph = rect->graphs().at(GraphId);
    }
    else if(GraphId == rect->graphs().length())
    {
        _graph = addGraph(rect->axis(QCPAxis::atBottom),rect->axis(QCPAxis::atLeft));
        rect->axis(QCPAxis::atBottom)->setRange(x.at(0),x.at(x.length()-1));
        rect->axis(QCPAxis::atLeft)->setRange(y.at(0),y.at(y.length()-1));

       setGraphColor(_graph,RectId+1);
    }
    else
    {
        qDebug()<< Q_FUNC_INFO << "GraphId值过大:" << GraphId << "应为："<<rect->graphs().length();
        return;
    }

    _graph->addData(x, y);

    autoScroll();

    qreplot();
}

//!
void QttCustomPlot::addData(QCPGraph* graph, double x, double y)
{
    if(graph != nullptr) graph->addData(x, y);

    autoScroll();
    qreplot();
}

//!
void QttCustomPlot::addData(QCPGraph* graph, QVector<double> x, QVector<double> y)
{
    if(graph != nullptr) graph->addData(x, y);

    autoScroll();

    qreplot();
}


//!
void QttCustomPlot::clearData(int RectId, int GraphId)
{
    if(RectId < 0 || RectId >= RectList.length())
    {
        qDebug() << Q_FUNC_INFO << "RectList index out of bounds:" << RectId;
        return;
    }

    auto rect = RectList.at(RectId);

    QCPGraph* _graph = nullptr;

    if(GraphId < 0)
    {
         qDebug()<< Q_FUNC_INFO << "rect.graphs() index cannot be attached:" << GraphId;
         return;
    }
    else if(GraphId < rect->graphs().length())
    {
        _graph = rect->graphs().at(GraphId);
    }
    else if(GraphId == rect->graphs().length())
    {
        _graph = addGraph(rect->axis(QCPAxis::atBottom),rect->axis(QCPAxis::atLeft));
    }
    else
    {
        qDebug()<< Q_FUNC_INFO << "GraphId值过大:" << GraphId << "应小于："<<rect->graphs().length();
        return;
    }

    _graph->data().data()->clear();
    removeGraph(_graph);

    qreplot();
}

//!
void QttCustomPlot::clearData(QCPGraph* graph)
{
    if(graph != nullptr)
    {
        graph->data().data()->clear();
        removeGraph(graph);
    }

    qreplot();
}


//!
void QttCustomPlot::when_selectionChangedByUser()
{
    for (int i = 0; i < rCount; ++i) {
        auto rect = RectList.at(i);
        for (int j = 0; j < rect->graphs().length(); ++j) {
            auto graph = rect->graphs().at(j);
            QCPPlottableLegendItem *item = legend->itemWithPlottable(graph);
            if (item->selected() || graph->selected())//选中了哪条曲线或者曲线的图例
            {
                item->setSelected(true);
                graph->selectionDecorator()->setPen(QPen(QColor(Qt::blue),2));
                graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
                return;
            }
        }
    }
}

//!
void QttCustomPlot::when_legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* legendItem, QMouseEvent* event)
{
    Q_UNUSED(legend);
    Q_UNUSED(event);

    for (int i = 0; i < this->graphCount(); ++i)
    {
        QCPGraph *pGraph = this->graph(i);
        if(legendItem == this->legend->itemWithPlottable(pGraph))//查询被点击的图例项是哪个graph的图例项
        {
            QColor newColor = QColorDialog::getColor(Qt::red,this,"选择新颜色");
            if(newColor.isValid())//用户点击了确定
            {
                pGraph->setPen(QPen(newColor,1));
                replot(QCustomPlot::rpQueuedReplot);
            }
        }
    }
}

void QttCustomPlot::qreplot()
{
    if(legendVisible)
        setLegendVisible(legendVisible);

    replot(QCustomPlot::rpQueuedReplot);
}

void QttCustomPlot::setAutoScroll(bool on)
{
    isAutoScroll = on;
}

void QttCustomPlot::autoScroll()
{
    if(isAutoScroll)
    {
        double curSeclf = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000.0;
        this->xAxis->setRange(curSeclf - xAxis->range().size(), curSeclf);
    }
}

//!
void QttCustomPlot::connectAllxAsix(bool on)
{
    for (int i = 0; i < rCount; ++i) {
        for (int j = i+1; j < rCount; ++j) {
            if(on)
            {
                connect(RectList.at(i)->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), RectList.at(j)->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
                connect(RectList.at(j)->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), RectList.at(i)->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
            }
            else
            {
                disconnect(RectList.at(i)->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), RectList.at(j)->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
                disconnect(RectList.at(j)->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), RectList.at(i)->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
            }

        }
    }
}


//!设置颜色
void QttCustomPlot::setGraphColor(QCPGraph* pGraph,int Chall_num)
{
   // pGraph->setPen(QPen(QColor(qrand()%256, qrand()%256, qrand()%256),1));
    switch(Chall_num)
    {
    case 1:
        pGraph->setPen(QPen(QColor(255,128,128)));
        break;
    case 2:
        pGraph->setPen(QPen(QColor(255,0,0)));
        break;
    case 3:
        pGraph->setPen(QPen(QColor(0,255,0)));
        break;
    case 4:
        pGraph->setPen(QPen(QColor(255,255,255)));
        break;
    case 5:
        pGraph->setPen(QPen(QColor(255,255,0)));
        break;
    case 6:

        pGraph->setPen(QPen(QColor(128,0,255)));
        break;
    case 7:
        pGraph->setPen(QPen(QColor(255,128,0)));
        break;
    case 8:
        pGraph->setPen(QPen(QColor(128,128,0)));
        break;
    case 9:
        pGraph->setPen(QPen(QColor(128,255,255)));
        break;
    case 10:
        pGraph->setPen(QPen(QColor(0,64,128)));
        break;
    case 11:
        pGraph->setPen(QPen(QColor(0,0,255)));
        break;
    case 12:
        pGraph->setPen(QPen(QColor(0,128,255)));
        break;
    case 13:
        pGraph->setPen(QPen(QColor(0,128,192)));
        break;
    case 14:
        pGraph->setPen(QPen(QColor(128,128,255)));
        break;
    case 15:
        pGraph->setPen(QPen(QColor(192,192,192)));
        break;
    case 16:
        pGraph->setPen(QPen(QColor(255,128,192)));;
        break;
    case 17:
        pGraph->setPen(QPen(QColor(128,0,64)));
        break;
    case 18:
        pGraph->setPen(QPen(QColor(255,0,255)));
        break;
    case 19:
        pGraph->setPen(QPen(QColor(0,64,0)));
        break;
    case 20:
        pGraph->setPen(QPen(QColor(79,79,79)));
        break;
    }
}



void QttCustomPlot::wheelEvent(QWheelEvent *e)
{
}




