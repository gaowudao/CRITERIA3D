#include "tabWaterRetentionCurve.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>

#include <QWidget>

TabWaterRetentionCurve::TabWaterRetentionCurve()
{
    // test
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *plotLayout = new QVBoxLayout;
    linesLayout = new QVBoxLayout;

    QwtPlot *myPlot = new QwtPlot;
    QwtPlotCurve *curve1 = new QwtPlotCurve;
    myPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));
    myPlot->setAxisTitle(QwtPlot::yLeft,QString("Water content [%1]").arg(QString("m^3/m^3")));
    myPlot->setAxisTitle(QwtPlot::xBottom,QString("Water potential [%1]").arg(QString("J/Kg")));
    myPlot->setAxisScale(QwtPlot::xBottom,0.01, 1000000);
    myPlot->setAxisAutoScale(QwtPlot::yLeft);
//    QwtPointSeriesData* myData = new QwtPointSeriesData;
//    QVector<QPointF>* samples = new QVector<QPointF>;
//    samples->push_back(QPointF(1.0,1.0));
//    samples->push_back(QPointF(2.0,2.0));
//    samples->push_back(QPointF(3.0,3.0));
//    samples->push_back(QPointF(4.0,5.0));
//    myData->setSamples(*samples);
//    curve1->setData(myData);

    curve1->attach(myPlot);

    mainLayout->addLayout(linesLayout);
    plotLayout->addWidget(myPlot);
    mainLayout->addLayout(plotLayout);
    setLayout(mainLayout);
}

void TabWaterRetentionCurve::insertVerticalLines(soil::Crit3DSoil *soil)
{
    mySoil = soil;
    int border = 30;
    int totWidth = 20;
    linesLayout->geometry().setWidth(totWidth);
    linesLayout->geometry().setHeight(this->geometry().height() - border);

    QRect layoutSize = linesLayout->geometry();
    int x1 = layoutSize.topLeft().x();
    int totHeight = layoutSize.height();

    for (int i = 0; i<mySoil->nrHorizons; i++)
    {

        int length = (mySoil->horizon[i].lowerDepth*100 - mySoil->horizon[i].upperDepth*100) * totHeight / (mySoil->totalDepth*100);
        LineHorizont* line = new LineHorizont();
        line->setFixedWidth(totWidth);
        line->setFixedHeight(length);
        qDebug() << "mySoil->horizon[i].texture.classUSDA " << mySoil->horizon[i].texture.classUSDA;
        line->setClass(mySoil->horizon[i].texture.classUSDA);
        linesLayout->addWidget(line);
        lineList.push_back(line);
    }

}

