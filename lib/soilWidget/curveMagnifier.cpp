#include "curveMagnifier.h"
#include "qdebug.h"

#include "qwt_plot.h"
#include "qwt_scale_div.h"


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

    const double center = scaleDiv->lowerBound() + scaleDiv->range() / 2;
    double width_2 = scaleDiv->range() / 2 * factor;
    if (factor == 1.1)
    {
        //y
        if (index == QwtPlot::yLeft)
        {

        }
                // x
//                if (index == QwtPlot::xBottom)
//                {
//
//                }
    }
    else if (factor == 0.9)
    {
                //y
        if (index == QwtPlot::yLeft)
        {

        }
        // x
//                if (index == QwtPlot::xBottom && width_2 > YMINWIDTH)
//                {
//                    width_2 = YMINWIDTH;
//                }
    }


    qwtPlot->setAxisScale(index, center - width_2, center + width_2);
    doReplot = true;

    qwtPlot->setAutoReplot(autoReplot);

    if ( doReplot )
        qwtPlot->replot();

}
