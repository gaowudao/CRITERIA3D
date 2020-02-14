#ifndef TABLAI_H
#define TABLAI_H

    #include <QtWidgets>
#ifndef METEOPOINT_H
    #include "meteoPoint.h"
#endif
#ifndef CROP_H
    #include "crop.h"
#endif
#include <QtCharts>

    class TabLAI : public QWidget
    {
        Q_OBJECT
    public:
        TabLAI();
        void computeLAI(Crit3DCrop* myCrop, Crit3DMeteoPoint *meteoPoint, int year, int nrLayers, double totalSoilDepth, int currentDoy);
        void drawLAI(Crit3DCrop* myCrop);
    private:
        int year;
        QChartView *chartView;
        QChart *chart;
        QLineSeries *series;
    };

#endif // TABLAI_H
