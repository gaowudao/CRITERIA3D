#include "curvePanner.h"
#include "qwt_scale_div.h"
#include "qwt_plot.h"
#include <math.h>


Crit3DCurvePanner::Crit3DCurvePanner(QwtPlot *plot, double dxMin, double dxMax, double dyMin, double dyMax) : QwtPlotPanner (plot->canvas()),
    qwtPlot(plot), dxMin(dxMin), dxMax(dxMax), dyMin(dyMin), dyMax(dyMax)
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
            if(d1 < dxMin)
            {
                d1 = dxMin;
                d2 = pow(10, (range + log10(d1)));
            }
            if(d2 > dxMax)
            {
                d2 = dxMax;
                d1 = pow(10, (log10(d2) - range));
            }
        }
        if (axis == QwtPlot::yLeft)
        {
            double range = d2 - d1;
            if(d1 < dyMin)
            {
                d1 = dyMin;
                d2 = range + d1;
            }
            if(d2 > dyMax)
            {
                d2 = dyMax;
                d1 = d2 - range;
            }
        }
        qwtPlot->setAxisScale(axis, d1, d2);
    }

    qwtPlot->setAutoReplot(doAutoReplot);
    qwtPlot->replot();
}
