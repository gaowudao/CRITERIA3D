#ifndef TABWATERRETENTIONCURVE_H
#define TABWATERRETENTIONCURVE_H

#include <QtWidgets>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include "soil.h"
#include "lineHorizont.h"
#include "myPicker.h"


class TabWaterRetentionCurve: public QWidget
{
    Q_OBJECT
public:
    TabWaterRetentionCurve();
    void insertElements(soil::Crit3DSoil* soil);

private:
    soil::Crit3DSoil* mySoil;
    QVBoxLayout* linesLayout;
    QwtPlot *myPlot;
    QList<LineHorizont*> lineList;
    QList<QwtPlotCurve*> curveList;
    MyPicker *pick;
private slots:
    void widgetClicked(int index);

};

#endif // WATERRETENTIONCURVE_H
