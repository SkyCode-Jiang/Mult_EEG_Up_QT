#ifndef FILTERTOOL_H
#define FILTERTOOL_H

#include <QObject>
#include <QDebug>
#include "fir_filter.h"
#include "include.h"

class FilterTool : public QObject
{
    Q_OBJECT
public:
    explicit FilterTool(QObject *parent = nullptr);

    void setFilter(double low_freq,double high_freq,double notch_lowfreq,double notch_highfreq,int fir_order,int fir_type);
    FIR_filter *fir[19];
    FIR_filter *notch_fir[19];

signals:

};

#endif // FILTERTOOL_H
