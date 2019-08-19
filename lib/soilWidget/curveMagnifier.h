#ifndef CURVEMAGNIFIER_H
#define CURVEMAGNIFIER_H

#include <QWidget>
#include <qwt_plot_magnifier.h>
#include <qwt_plot.h>

class Crit3DCurveMagnifier : public QwtPlotMagnifier
{
    Q_OBJECT
public:
    Crit3DCurveMagnifier(QwtPlot *plot, int axisIndex);
    virtual void rescale(double factor);
private:
    QwtPlot *qwtPlot;
    int index;
};

#endif // CURVEMAGNIFIER_H
