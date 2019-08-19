#include "curvePanner.h"
#include "qwt_plot.h"

Crit3DCurvePanner::Crit3DCurvePanner(QwtPlot *plot) : QwtPlotPanner (plot->canvas()), qwtPlot(plot)
{

}
