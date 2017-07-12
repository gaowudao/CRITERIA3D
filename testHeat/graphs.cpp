#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_dict.h>

#include "gis.h"
#include "graphs.h"
#include "commonConstants.h"

Plot::Plot( QWidget *parent ):
    QwtPlot( parent )
{
    canvas()->setStyleSheet(
        "border: 1px solid Black;"
        "border-radius: 15px;"
        "background-color: white;"
    );

    // panning with the left mouse button
    (void )new QwtPlotPanner( canvas() );

    // zoom in/out with the wheel
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier( canvas() );
    magnifier->setMouseButton( Qt::NoButton );

    QwtLegend *legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
    this->insertLegend(legend, QwtPlot::BottomLegend);
}

QwtPlotGappedCurve::QwtPlotGappedCurve(const QString &title, double gapValue)
:	QwtPlotCurve(title),
    gapValue_(gapValue)
{
}

void QwtPlotGappedCurve::drawSeries(QPainter *painter, const QwtScaleMap &xMap,
                                                          const QwtScaleMap &yMap, const QRectF &canvRect, int from, int to) const
{
    if ( !painter || (int)dataSize() <= 0 )
        return;

    if (to < 0)
        to = (int)dataSize() - 1;

    int i = from;
    while (i < to)
    {
        // If data begins with missed values,
        // we need to find first non-missed point.
            double y = sample(i).y();
            while ((i < to) && (y == gapValue_))
            {
                ++i;
                y = sample(i).y();
            }
        // First non-missed point will be the start of curve section.
            int start = i;
            y = sample(i).y();
        // Find the last non-missed point, it will be the end of curve section.
            while ((i < to) && (y != gapValue_))
            {
                ++i;
                y = sample(i).y();
            }
        // Correct the end of the section if it is at missed point
            int end = (y == gapValue_) ? i - 1 : i;

        // Draw the curve section
            if (start <= end)
                    QwtPlotCurve::drawSeries(painter, xMap, yMap, canvRect, start, end);
    }
}


void Plot::addCurve(QString myTitle, QwtPlotGappedCurve::CurveStyle myStyle, QPen myPen, QVector<QPointF> &samples)
{
    QwtPlotGappedCurve* myCurve = new QwtPlotGappedCurve(myTitle, NODATA);

    myCurve->setPen(myPen);
    myCurve->setStyle(myStyle);
    myCurve->setSamples(samples);
    myCurve->attach( this );
}

QVector<QPointF> getProfileSeries(heat_output* myOut, outputType myVar, int layerIndex)
{
    QVector<QPointF> mySeries;
    QPointF myPoint;

    switch (myVar)
    {
        case outputType::soilTemperature :
            for (int i=0; i<myOut->nrValues; i++)
            {
                myPoint.setX(i);
                myPoint.setY(myOut->profileOutput[i].temperature[layerIndex].y());
                mySeries.push_back(myPoint);
            }
        break;

        case outputType::soilWater :
            for (int i=0; i<myOut->nrValues; i++)
            {
                myPoint.setX(i);
                myPoint.setY(myOut->profileOutput[i].waterContent[layerIndex].y());
                mySeries.push_back(myPoint);
            }
        break;

        case outputType::totalHeatFlux :
            for (int i=0; i<myOut->nrValues; i++)
            {
                myPoint.setX(i);
                myPoint.setY(myOut->profileOutput[i].totalHeatFlux[layerIndex].y());
                mySeries.push_back(myPoint);
            }
        break;

        case outputType::latentHeatFluxIso :
            for (int i=0; i<myOut->nrValues; i++)
            {
                myPoint.setX(i);
                myPoint.setY(myOut->profileOutput[i].isothermalLatentHeatFlux[layerIndex].y());
                mySeries.push_back(myPoint);
            }
        break;

        case outputType::latentHeatFluxTherm :
            for (int i=0; i<myOut->nrValues; i++)
            {
                myPoint.setX(i);
                myPoint.setY(myOut->profileOutput[i].thermalLatentHeatFlux[layerIndex].y());
                mySeries.push_back(myPoint);
            }
        break;

        case outputType::diffusiveHeatFlux :
            for (int i=0; i<myOut->nrValues; i++)
            {
                myPoint.setX(i);
                myPoint.setY(myOut->profileOutput[i].diffusiveHeatFlux[layerIndex].y());
                mySeries.push_back(myPoint);
            }
        break;

    }

    for (int i=50; i<100; i++)
        mySeries[i].setY(NODATA);

    return mySeries;
}

void Plot::drawProfile(outputType graphType, heat_output* myOut)
{
    detachItems(QwtPlotItem::Rtti_PlotCurve, true);

    QPen myPen;

    Crit3DColorScale myColorScale;
    myColorScale.nrKeyColors = 3;
    myColorScale.nrColors = 256;
    myColorScale.keyColor = new Crit3DColor[myColorScale.nrKeyColors];
    myColorScale.color = new Crit3DColor[myColorScale.nrColors];
    myColorScale.classification = classificationMethod::EqualInterval;
    myColorScale.keyColor[0] = Crit3DColor(0, 0, 255);         /*!< blue */
    myColorScale.keyColor[1] = Crit3DColor(255, 255, 0);       /*!< yellow */
    myColorScale.keyColor[2] = Crit3DColor(255, 0, 0);         /*!< red */
    myColorScale.minimum = 0;
    myColorScale.maximum = myOut->nrLayers-1;
    myColorScale.classify();

    Crit3DColor* myColor;
    QColor myQColor;

    float myDepth;

    QVector<QPointF> mySeries;

    for (int z=0; z<myOut->nrLayers; z++)
    {
        myDepth = myOut->layerThickness * ((float)z + 0.5);

        myColor = myColorScale.getColor(z);
        myQColor = QColor(myColor->red, myColor->green, myColor->blue);
        myPen.setColor(myQColor);

        mySeries = getProfileSeries(myOut, graphType, z);
        addCurve(QString::number(myDepth,'f',3), QwtPlotCurve::Lines, myPen, mySeries);
    }

    this->setAxisScale();
    replot();
}








