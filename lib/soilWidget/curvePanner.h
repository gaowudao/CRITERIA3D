#ifndef CURVEPANNER_H
#define CURVEPANNER_H

    #include <QWidget>
    #include "qwt_plot_panner.h"

    #define DXMIN 0.001
    #define DXMAX 1000000
    #define DYMIN 0.0
    #define DYMAX 1.0

    #define XMIN (DXMIN * 100)
    #define XMAX (DXMAX / 100)
    #define YMIN 0.0
    #define YMAX 0.6

    class Crit3DCurvePanner : public QwtPlotPanner
    {
        Q_OBJECT
    public:
        Crit3DCurvePanner(QwtPlot *plot);
        void moveCanvas(int dx, int dy);

    private:
        QwtPlot *qwtPlot;

    };

#endif // CURVEPANNER_H
