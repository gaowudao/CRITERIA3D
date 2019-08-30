#ifndef CURVEPANNER_H
#define CURVEPANNER_H

    #include <QWidget>
    #include "qwt_plot_panner.h"

    class Crit3DCurvePanner : public QwtPlotPanner
    {
        Q_OBJECT
    public:
        Crit3DCurvePanner(QwtPlot *plot, double dxMin, double dxMax, double dyMin, double dyMax);
        void moveCanvas(int dx, int dy);

    private:
        QwtPlot *qwtPlot;
        double dxMin;
        double dxMax;
        double dyMin;
        double dyMax;

    };

#endif // CURVEPANNER_H
