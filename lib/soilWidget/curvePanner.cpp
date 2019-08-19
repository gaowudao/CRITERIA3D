#include "curvePanner.h"
#include "qwt_scale_div.h"
#include "qwt_plot.h"

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
//        if ( !d_data->isAxisEnabled[axis] )
//            continue;

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
        qDebug() << "d1 " << d1;
        qDebug() << "d2 " << d2;
        qwtPlot->setAxisScale(axis, d1, d2);
    }

    qwtPlot->setAutoReplot(doAutoReplot);
    qwtPlot->replot();
}
