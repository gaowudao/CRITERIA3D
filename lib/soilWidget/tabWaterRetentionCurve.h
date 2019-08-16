#ifndef TABWATERRETENTIONCURVE_H
#define TABWATERRETENTIONCURVE_H

    #include <QtWidgets>
    #include <qwt_plot_curve.h>
    #include <qwt_plot.h>
    #include "soil.h"
    #include "lineHorizont.h"
    #include "curvePicker.h"


    class TabWaterRetentionCurve: public QWidget
    {
        Q_OBJECT
    public:
        TabWaterRetentionCurve();
        void insertElements(soil::Crit3DSoil* soil);
        void resetAll();
        bool getFillElement() const;
        void setFillElement(bool value);

    private:
        soil::Crit3DSoil* mySoil;
        QVBoxLayout* linesLayout;
        QwtPlot *myPlot;
        QList<LineHorizont*> lineList;
        QList<QwtPlotCurve*> curveList;
        Crit3CurvePicker *pick;
        bool fillElement;

    private slots:
        void widgetClicked(int index);
        void curveClicked(int index);

    signals:
        void horizonSelected(int nHorizon);

    };

#endif // WATERRETENTIONCURVE_H
