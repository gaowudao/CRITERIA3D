#include "tabWaterRetentionCurve.h"

#include "commonConstants.h"
#include <qwt_point_data.h>
#include <qwt_scale_engine.h>
#include <QWidget>

#define XMIN 0.01
#define XMAX 1000000
#define YMIN 0.01
#define YMAX 0.6

TabWaterRetentionCurve::TabWaterRetentionCurve()
{
    qDebug() << "TabWaterRetentionCurve constructor";
    // test
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *plotLayout = new QVBoxLayout;
    linesLayout = new QVBoxLayout;

    myPlot = new QwtPlot;
    myPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));
    myPlot->setAxisTitle(QwtPlot::yLeft,QString("Water content [%1]").arg(QString("m^3/m^3")));
    myPlot->setAxisTitle(QwtPlot::xBottom,QString("Water potential [%1]").arg(QString("J/Kg")));
    myPlot->setAxisScale(QwtPlot::xBottom,XMIN, XMAX);
    myPlot->setAxisScale(QwtPlot::yLeft,YMIN, YMAX);

//    QwtPointSeriesData* myData = new QwtPointSeriesData;
//    QVector<QPointF>* samples = new QVector<QPointF>;
//    samples->push_back(QPointF(1.0,1.0));
//    samples->push_back(QPointF(2.0,2.0));
//    samples->push_back(QPointF(3.0,3.0));
//    samples->push_back(QPointF(4.0,5.0));
//    myData->setSamples(*samples);
//    curve1->setData(myData);

//    curve1->attach(myPlot);

    mainLayout->addLayout(linesLayout);
    plotLayout->addWidget(myPlot);
    mainLayout->addLayout(plotLayout);

    setLayout(mainLayout);

}

void TabWaterRetentionCurve::insertElements(soil::Crit3DSoil *soil)
{

    QRect layoutSize = linesLayout->geometry();

    int x1 = layoutSize.topLeft().x();
    int totHeight = layoutSize.height();

    if (soil!=nullptr)
    {
        mySoil = soil;
        // delete all Widgets
        if (!lineList.isEmpty())
        {
            qDebug() << "delete all Widgets";
            qDeleteAll(lineList);
            lineList.clear();

            qDeleteAll(curveList);
            curveList.clear();
        }
        QVector<double> xVector;
        QVector<double> yVector;
        double x;

        for (int i = 0; i<mySoil->nrHorizons; i++)
        {

            // insertVerticalLines
            int length = (mySoil->horizon[i].lowerDepth*100 - mySoil->horizon[i].upperDepth*100) * totHeight / (mySoil->totalDepth*100);
            LineHorizont* line = new LineHorizont();
            line->setFixedWidth(20);
            line->setFixedHeight(length);
            line->setClass(mySoil->horizon[i].texture.classUSDA);
            linesLayout->addWidget(line);
            lineList.push_back(line);

            // insert Curves
            QwtPlotCurve *curve = new QwtPlotCurve;
            xVector.clear();
            yVector.clear();
            x = XMIN;
            while (x < XMAX)
            {
                double y = soil::thetaFromSignPsi(-x, &mySoil->horizon[i]);
                if (y != NODATA)
                {
                    xVector.push_back(x);
                    yVector.push_back(y);
                }
                x=x*1.25;
            }
            QwtPointArrayData *data = new QwtPointArrayData(xVector,yVector);
            curve->setSamples(data);
            curve->attach(myPlot);
            curveList.push_back(curve);
        }
        linesLayout->update();
        myPlot->replot();
    }

}


