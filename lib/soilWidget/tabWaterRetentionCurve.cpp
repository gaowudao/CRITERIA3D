#include "tabWaterRetentionCurve.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>

#include <QWidget>

TabWaterRetentionCurve::TabWaterRetentionCurve()
{
    // test
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QwtPlot *myPlot = new QwtPlot;
    QwtPlotCurve *curve1 = new QwtPlotCurve;
    QwtPointSeriesData* myData = new QwtPointSeriesData;
    QVector<QPointF>* samples = new QVector<QPointF>;
    myPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));
    myPlot->setAxisScale(QwtPlot::yLeft,1, 100);
//    samples->push_back(QPointF(1.0,1.0));
//    samples->push_back(QPointF(2.0,2.0));
//    samples->push_back(QPointF(3.0,3.0));
//    samples->push_back(QPointF(4.0,5.0));
    myData->setSamples(*samples);
    curve1->setData(myData);

    curve1->attach(myPlot);
    mainLayout->addWidget(myPlot);
    setLayout(mainLayout);
}
