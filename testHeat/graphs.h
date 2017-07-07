#ifndef GRAPHS_H
#define GRAPHS_H

#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include <qdatetime.h>

class QwtPlotCurve;
class QwtSymbol;

enum outputType
{
    soilTemperature = 0,
    soilWater = 1
};

struct profileStatus{
    QVector<QPointF> temperature;
    QVector<QPointF> waterContent;
    QVector<QPointF> totalHeatFlux;
    QVector<QPointF> diffusiveHeatFlux;
    QVector<QPointF> isothermalLatentHeatFlux;
    QVector<QPointF> thermalLatentHeatFlux;
    QVector<QPointF> advectiveheatFlux;
};

struct landSurfaceStatus{
    QPointF sensibleHeat;
    QPointF latentHeat;
    QPointF netRadiation;
    QPointF aeroConductance;
    QPointF soilConductance;
};

struct heatErrors{
    QDateTime myTime;
    QPointF heatMBR;
    QPointF heatMBE;
    QPointF waterMBR;
};

class heat_output
{
public:

    heat_output();

    int nrValues;
    int nrLayers;

    QVector<landSurfaceStatus> landSurfaceOutput;
    QVector<profileStatus> profileOutput;
    QVector<heatErrors> errorOutput;

    QString getTextOutput();
};

class Plot : public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget *parent = NULL );

    void addCurve(QString myTitle, QwtPlotCurve::CurveStyle myStyle, QwtSymbol* mySymbol, QVector<QPointF> &samples);
    void drawProfile(outputType graphType, heat_output* myOut);
};

#endif // GRAPHS_H
