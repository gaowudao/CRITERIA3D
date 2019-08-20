#include "curvePanner.h"
#include "qwt_scale_div.h"
#include "qwt_plot.h"
#include <math.h>

#define DXMIN 0.0001
#define DXMAX 10000000
#define DYMIN -0.2
#define DYMAX 1.2

Crit3DCurvePanner::Crit3DCurvePanner(QwtPlot *plot) : QwtPlotPanner (plot->canvas()), qwtPlot(plot)
{

}

void Crit3DCurvePanner::moveCanvas(int dx, int dy)
{
    if ( dx == 0 && dy == 0 )
        return;

    if ( qwtPlot == nullptr )
        return;

    const bool doAutoReplot = qwtPlot->autoReplot();
    qwtPlot->setAutoReplot(false);

    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        const QwtScaleMap map = qwtPlot->canvasMap(axis);

        const QwtScaleDiv *scaleDiv;
        scaleDiv = &qwtPlot->axisScaleDiv(axis);
        const int i1 = map.transform(scaleDiv->lowerBound());
        const int i2 = map.transform(scaleDiv->upperBound());

        double d1, d2;
        if ( axis == QwtPlot::xBottom || axis == QwtPlot::xTop )
        {
            d1 = map.invTransform(i1 - dx);
            d2 = map.invTransform(i2 - dx);
        }
        else
        {
            d1 = map.invTransform(i1 - dy);
            d2 = map.invTransform(i2 - dy);
        }
        double range;
        if (axis == QwtPlot::xBottom)
        {
            range = log10(d2) - log10(d1);
            if(d1 < DXMIN)
            {
                d1 = DXMIN;
                d2 = pow(10, (range + log10(d1)));
            }
            if(d2 > DXMAX)
            {
                d2 = DXMAX;
                d1 = pow(10, (log10(d2) - range));
            }
        }
        if (axis == QwtPlot::yLeft)
        {
            double range = d2 - d1;
            if(d1 < DYMIN)
            {
                d1 = DYMIN;
                d2 = range + d1;
            }
            if(d2 > DYMAX)
            {
                d2 = DYMAX;
                d1 = d2 - range;
            }
        }
        qwtPlot->setAxisScale(axis, d1, d2);
    }

    qwtPlot->setAutoReplot(doAutoReplot);
    qwtPlot->replot();
}
