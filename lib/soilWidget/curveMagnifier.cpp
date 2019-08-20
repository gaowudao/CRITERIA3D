#include "curveMagnifier.h"

#include "qwt_plot.h"
#include "qwt_scale_div.h"

#define YMAXRANGE 2
#define YMINRANGE 0.3
#define YMINLOWERBOUND -0.5

Crit3DCurveMagnifier::Crit3DCurveMagnifier(QwtPlot *plot, int axisIndex) : QwtPlotMagnifier(plot->canvas()), qwtPlot(plot), index(axisIndex)
{

}

void Crit3DCurveMagnifier::rescale(double factor)
{

    factor = qAbs(factor);
    if ( factor == 1.0 || factor == 0.0 )
        return;
    bool doReplot = false;

    const bool autoReplot = qwtPlot->autoReplot();
    qwtPlot->setAutoReplot(false);


    const QwtScaleDiv *scaleDiv;
    scaleDiv = &qwtPlot->axisScaleDiv(index);

    double center = scaleDiv->lowerBound() + scaleDiv->range() / 2;
    double width_2 = scaleDiv->range() / 2 * factor;
    //y
    if (index == QwtPlot::yLeft)
    {
        if (scaleDiv->range() > YMAXRANGE || scaleDiv->lowerBound() < YMINLOWERBOUND)
        {
            width_2 = YMAXRANGE / 2 * factor;
            center = width_2 / 2;
            qwtPlot->setAxisScale(index, center - width_2, center + width_2);
        }
        else if(scaleDiv->range() < YMINRANGE)
        {
            width_2 = YMINRANGE / 2 * factor;
            center = width_2 / 2;
            qwtPlot->setAxisScale(index, center - width_2, center + width_2);
        }
        else
        {
            qwtPlot->setAxisScale(index, center - width_2, center + width_2);
        }
    }

    qwtPlot->setAxisScale(index, center - width_2, center + width_2);
    doReplot = true;

    qwtPlot->setAutoReplot(autoReplot);

    if ( doReplot )
        qwtPlot->replot();

}
