#include "tabWaterRetentionCurve.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>

#include <QWidget>

TabWaterRetentionCurve::TabWaterRetentionCurve()
{
    qDebug() << "TabWaterRetentionCurve constructor";
    // test
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *plotLayout = new QVBoxLayout;
    linesLayout = new QVBoxLayout;
//    linesLayout->setMargin(0);
//    linesLayout->setContentsMargins(QMargins(0,0,0,0));
//    linesLayout->setSpacing(0);

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
//    mainLayout->setSpacing(0);

    setLayout(mainLayout);

}

void TabWaterRetentionCurve::insertVerticalLines(soil::Crit3DSoil *soil)
{

    mySoil = soil;
    QRect layoutSize = linesLayout->geometry();

    int x1 = layoutSize.topLeft().x();
    int totHeight = layoutSize.height();

//    qDebug() << "this->geometry().height() " << this->geometry().height();
//    qDebug() << "linesLayout->geometry() " << linesLayout->geometry();
//    qDebug() << "this->layout()->geometry() " << this->layout()->geometry();
//    qDebug() << "layoutSize.topLeft().x() " << layoutSize.topLeft().x();
//    qDebug() << "layoutSize.height() " << layoutSize.height();

    // delete all Widgets
    if (!lineList.isEmpty())
    {
        qDebug() << "delete all Widgets";
        qDeleteAll(lineList);
        lineList.clear();
    }

    for (int i = 0; i<mySoil->nrHorizons; i++)
    {

        int length = (mySoil->horizon[i].lowerDepth*100 - mySoil->horizon[i].upperDepth*100) * totHeight / (mySoil->totalDepth*100);
        LineHorizont* line = new LineHorizont();
        line->setFixedWidth(20);
        line->setFixedHeight(length);
        qDebug() << "mySoil->horizon[i].texture.classUSDA " << mySoil->horizon[i].texture.classUSDA << " length " << length;
        line->setClass(mySoil->horizon[i].texture.classUSDA);
        linesLayout->addWidget(line);
        lineList.push_back(line);
    }
    linesLayout->update();
    this->repaint();

}

