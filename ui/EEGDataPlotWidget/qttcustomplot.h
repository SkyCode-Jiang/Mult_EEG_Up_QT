#ifndef QTTCUSTOMPLOT_H
#define QTTCUSTOMPLOT_H

#include <QObject>
#include "qcptooltip.h"
#include "extern/qcustomplot.h"
#define AppFontName "Microsoft YaHei"
#define AppFontSize 9
#define TextColor QColor(255,255,255)
#define TextWidth 1
#define StartX (-0.1)
class QttCustomPlot : public QCustomPlot
{
    Q_OBJECT


public:
    void wheelEvent(QWheelEvent *e);
    explicit QttCustomPlot(int count);
    //! legend
    void setLegendVisible(bool on);
    //!
    void addRect(int idx);
    //!
    void removeRect(int idx);
    //!
    void hideAxis(int rectId, QCPAxis::AxisType type, bool on = false);
    //!
    void hideAxises(bool on = false);
    //!
    QCPGraph* getGraph(int RectId, int GraphId);
    //!
    void addData(int RectId, int GraphId, double x, double y);
    //!
    void addData(int RectId, int GraphId, QVector<double> x, QVector<double> y);
    //!
    void addData(QCPGraph* graph, double x, double y);
    //!
    void addData(QCPGraph* graph, QVector<double> x, QVector<double> y);
    //!
    void clearData(int RectId, int GraphId);
    //!
    void clearData(QCPGraph* graph);
    //!
    void setAutoScroll(bool on);
    //!
    void connectAllxAsix(bool on);
    //!
    void setGraphColor(QCPGraph* pGraph,int Chall_num);
     void qreplot();

    void setLegen(int RectId, int GraphId,QString str);
    QList<QCPAxisRect *> RectList;
    QCPLegend* My_legend;
    int Plot_chall;

signals:


private slots:
    void when_selectionChangedByUser();

    void when_legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*);


private:
    QCPToolTip* toolTip;
    //
    int rCount;
    //

    //
    bool AxisHide;
    // 隐藏legend及其所在布局LegendLayoutGrid
    bool legendVisible;

    //
    QCPLayoutGrid* plotLayoutGrid;
    QCPLayoutGrid* legendLayoutGrid;

    //
    QCPLegend* legend;

    //
    bool isAutoScroll;


    //!
    void init_customplot();

    //!
    void init_LayoutGrid();
    //!
    void init_Rect();
    //!
    void init_legend();



    //!


    //!
    void autoScroll();


};

#endif // QTTCUSTOMPLOT_H
