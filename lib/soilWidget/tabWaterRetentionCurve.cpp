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
    linesLabel = new QLabel;
    linesLabel->size().setHeight(mainLayout->geometry().height());
    linesLabel->setMinimumWidth(10);
    linesLabel->setMaximumWidth(20);

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

    mainLayout->addWidget(linesLabel);
    plotLayout->addWidget(myPlot);
    mainLayout->addLayout(plotLayout);
    setLayout(mainLayout);
}

void TabWaterRetentionCurve::insertVerticalLines(soil::Crit3DSoil *soil)
{
    mySoil = soil;
    QRect labelSize = linesLabel->geometry();
    LineHorizont* line;
    qDebug() << "linesLabel.topLeft().x() " << labelSize.topLeft().x();
    qDebug() << "linesLabel.topLeft().y() " << labelSize.topLeft().y();
    qDebug() << "linesLabel.w " << linesLabel->width();
    qDebug() << "linesLabel.h " << linesLabel->height();
    painter.begin(linesLabel);
    for (int i = 0; i<mySoil->nrHorizons; i++)
    {
        int length = (mySoil->horizon[i].upperDepth-mySoil->horizon[i].lowerDepth) / labelSize.y();
        if (i == 0)
        {
            line = new LineHorizont(labelSize.topLeft().x(),labelSize.topLeft().y(),labelSize.topLeft().x(), labelSize.topLeft().y() + length,"prova");
        }
        else
        {
            line = new LineHorizont(labelSize.topLeft().x(),lineList.at(i-1)->y2(),labelSize.topLeft().x(), lineList.at(i-1)->y2() + length,"prova");
        }

        lineList.push_back(line);
        painter.drawLine(*line);
    }
    painter.end();
}

