#ifndef MAPWIDGET_H
#define MAPWIDGET_H

    #ifndef QWIDGET_H
        #include <QWidget>
    #endif
    #ifndef GIS_H
        #include "gis.h"
    #endif

    enum TMapVariable {airTempMap, precMap, relHumMap, windIntMap, leafWetMap,
                       pressMap, airTransMap, globRadMap, beamRadMap, diffRadMap, reflRadMap,
                       dtmMap};


    class mapWidget : public QWidget
    {
        Q_OBJECT
    public:
        mapWidget();

        float zoomFactor;
        float minZoomFactor;

        void setCurrentView(TMapVariable myVar);

    private:
        void redrawMap(gis::Crit3DRasterGrid* myGrid);
        void redrawMeteoPoints();
        void initializeWindow();
        TMapVariable currentViewVariable;

    protected:
        void paintEvent(QPaintEvent *event);
    signals:

    public slots:

    };

#endif

