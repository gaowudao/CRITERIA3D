#ifndef SHAREDMAINWINDOW_H
#define SHAREDMAINWINDOW_H

    #include <QMainWindow>
    #include <QList>
    #include <QActionGroup>

    #include "tileSources/OSMTileSource.h"
    #include "Position.h"
    #include "rubberBand.h"
    #include "MapGraphicsView.h"
    #include "MapGraphicsScene.h"
    #include "stationMarker.h"
    #include "mapGraphicsRasterObject.h"
    #include "colorLegend.h"
    #include "viewer3d.h"


    enum visualizationType {notShown, showLocation, showCurrentVariable, showElaboration,
                            showAnomalyAbsolute, showAnomalyPercentage, showClimate};

    enum mapType {mapNone, mapDEM, mapSoil, mapVariable};


    class SharedMainWindow : public QMainWindow
    {
        Q_OBJECT

    protected slots:
        void on_rasterOpacitySlider_sliderMoved(int position);
        void on_meteoGridOpacitySlider_sliderMoved(int position);


    protected:
        Position* startCenter;
        MapGraphicsScene* mapScene;
        MapGraphicsView* mapView;
        RasterObject* rasterObj;
        RasterObject* meteoGridObj;
        ColorLegend *rasterLegend;
        ColorLegend *meteoPointsLegend;
        ColorLegend *meteoGridLegend;
        QList<StationMarker*> pointList;
        RubberBand *myRubberBand;
        visualizationType currentPointsVisualization;
        QActionGroup *showPointsGroup;

        void addMeteoPoints();

    };

#endif // SHAREDMAINWINDOW_H
