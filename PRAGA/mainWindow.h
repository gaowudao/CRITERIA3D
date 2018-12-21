#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QList>
    #include <QCheckBox>
    #include <QGroupBox>
    #include <QActionGroup>

    #include "Position.h"
    #include "rubberBand.h"
    #include "MapGraphicsView.h"
    #include "MapGraphicsScene.h"
    #include "tileSources/OSMTileSource.h"
    #include "rasterObject.h"
    #include "stationMarker.h"
    #include "colorlegend.h"
    #include "dbArkimet.h"

    enum visualizationType {notShown, showLocation, showCurrentVariable, showElaboration, showAnomalyAbsolute, showAnomalyPercentage, showClimate};

    namespace Ui
    {
        class MainWindow;
    }

    /*!
     * \brief The MainWindow class
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:

        explicit MainWindow( QWidget *parent = 0);
        ~MainWindow();

    private slots:

        void on_actionLoadDEM_triggered();
        void on_actionNewMeteoPointsArkimet_triggered();
        void on_actionOpen_meteo_points_DB_triggered();
        void on_actionOpen_meteo_grid_triggered();
        void on_actionDownload_meteo_data_triggered();
        void on_rasterOpacitySlider_sliderMoved(int position);
        void on_meteoGridOpacitySlider_sliderMoved(int position);
        void on_actionMapToner_triggered();
        void on_actionMapOpenStreetMap_triggered();
        void on_actionMapESRISatellite_triggered();
        void on_actionMapTerrain_triggered();
        void on_actionRectangle_Selection_triggered();
        void on_dateChanged();
        void on_rasterScaleButton_clicked();
        void on_variableButton_clicked();
        void on_frequencyButton_clicked();
        void enableAllDataset(bool toggled);
        void disableAllButton(bool toggled);
        void on_actionVariableQualitySpatial_triggered();
        void on_rasterRestoreButton_clicked();
        void on_timeEdit_timeChanged(const QTime &time);
        void on_dateEdit_dateChanged(const QDate &date);
        void on_actionClose_meteo_points_triggered();
        void on_actionClose_meteo_grid_triggered();
        void on_actionInterpolation_to_DTM_triggered();
        void on_actionInterpolation_to_Grid_triggered();
        void on_actionSave_meteo_grid_triggered();
        void on_actionCompute_elaboration_triggered();
        void on_actionCompute_anomaly_triggered();
        void on_actionCompute_climate_triggered();
        void on_actionInterpolationSettings_triggered();

    #ifdef NETCDF
        void on_actionOpen_NetCDF_data_triggered();
        void on_actionExtract_NetCDF_series_triggered();
    #endif

        void on_actionParameters_triggered();
        void on_actionWriteTAD_triggered();
        void on_actionLoadTAD_triggered();
        void on_actionClimate_fields_triggered();

        void on_actionShowPointsHide_triggered();
        void on_actionShowPointsLocation_triggered();
        void on_actionShowPointsCurrent_triggered();

        void on_actionShowGridHide_triggered();
        void on_actionShowGridLocation_triggered();
        void on_actionShowGridCurrent_triggered();

        void on_actionShowPointsElab_triggered();

        void on_actionShowGridElab_triggered();

        void on_meteoPoints_clicked();

        void on_grid_clicked();

        void on_actionShowPointsAnomalyAbs_triggered();

        void on_actionShowGridAnomalyAbs_triggered();

        void on_actionShowPointsAnomalyPerc_triggered();

        void on_actionShowGridAnomalyPerc_triggered();

        void on_actionShowPointsClimate_triggered();

        void on_actionShowGridClimate_triggered();

    protected:
        /*!
         * \brief mouseReleaseEvent call moveCenter
         * \param event
         */
        void mouseReleaseEvent(QMouseEvent *event);

        /*!
         * \brief mouseDoubleClickEvent implements zoom In and zoom Out
         * \param event
         */
        void mouseDoubleClickEvent(QMouseEvent * event);

        void mouseMoveEvent(QMouseEvent * event);

        void mousePressEvent(QMouseEvent *event);

        void resizeEvent(QResizeEvent * event);

    private:
        Ui::MainWindow* ui;

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
        visualizationType currentGridVisualization;
        QActionGroup *showPointsGroup;
        QActionGroup *showGridGroup;

        QList<QCheckBox*> datasetCheckbox;
        QCheckBox* all;

        /*
        QLineEdit* elabType1;
        QLineEdit* elabType2;
        QLineEdit* elabVariable;
        QLineEdit* elabPeriod;
        */

        void setMapSource(OSMTileSource::OSMTileType mySource);
        QString selectArkimetDataset(QDialog* datasetDialog);

        QPoint getMapPoint(QPoint* point) const;

        void updateVariable();
        void updateDateTime();
        void resetMeteoPoints();
        void addMeteoPoints();
        void redrawMeteoPoints(visualizationType showType, bool updateColorSCale);
        void redrawMeteoGrid(visualizationType showType);

        bool loadMeteoPointsDB(QString dbName);
        bool loadMeteoGridDB(QString xmlName);
        void setCurrentRaster(gis::Crit3DRasterGrid *myRaster);
        void interpolateDemGUI();
        void interpolateGridGUI();
        void showElabResult(bool updateColorSCale, bool isMeteoGrid, bool isAnomaly, bool isAnomalyPerc, bool isClima, QString index);
    };


    #ifdef NETCDF
        void exportNetCDFDataSeries(gis::Crit3DGeoPoint geoPoint);
    #endif


#endif // MAINWINDOW_H
