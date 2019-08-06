#ifndef SHAREDMAINWINDOW_H
#define SHAREDMAINWINDOW_H

    #include "tileSources/OSMTileSource.h"
    #include "Position.h"
    #include "MapGraphicsView.h"
    #include "MapGraphicsScene.h"

    #include "mapGraphicsRasterObject.h"
    #include "stationMarker.h"
    #include "rubberBand.h"
    #include "colorLegend.h"

    #include <QMainWindow>
    class QActionGroup;


    enum visualizationType {notShown, showLocation, showCurrentVariable, showElaboration,
                            showAnomalyAbsolute, showAnomalyPercentage, showClimate};


    class SharedMainWindow : public QMainWindow
    {
        Q_OBJECT

    protected:
        Position* startCenter;
        MapGraphicsScene* mapScene;
        MapGraphicsView* mapView;

        RasterObject* rasterObj;
        RasterObject* meteoGridObj;
        QList<StationMarker*> pointList;
        RubberBand *myRubberBand;

        ColorLegend *rasterLegend;
        ColorLegend *meteoPointsLegend;
        ColorLegend *meteoGridLegend;

        QActionGroup *showPointsGroup;
        QActionGroup *showGridGroup;

        visualizationType currentPointsVisualization;
        visualizationType currentGridVisualization;

    };


#endif // SHAREDMAINWINDOW_H
