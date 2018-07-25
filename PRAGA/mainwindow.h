#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QList>
    #include <QCheckBox>
    #include <QGroupBox>

    #include "rubberBand.h"
    #include "MapGraphicsView.h"
    #include "MapGraphicsScene.h"
    #include "tileSources/OSMTileSource.h"
    #include "rasterObject.h"
    #include "stationMarker.h"
    #include "colorlegend.h"
    #include "guiConfiguration.h"
    #include "dbArkimet.h"


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

        explicit MainWindow(environment menu, QWidget *parent = 0);
        ~MainWindow();

    private slots:

        void on_actionLoadRaster_triggered();

        void on_actionNewMeteoPointsArkimet_triggered();

        void on_actionOpen_meteo_points_DB_triggered();

        void on_actionOpen_meteo_grid_triggered();

        void on_actionDownload_meteo_data_triggered();

        void on_rasterOpacitySlider_sliderMoved(int position);

        void on_actionMapToner_triggered();

        void on_actionMapOpenStreetMap_triggered();

        void on_actionMapESRISatellite_triggered();

        void on_actionMapTerrain_triggered();

        void on_actionSetUTMzone_triggered();

        void on_actionRectangle_Selection_triggered();

        void on_actionVariableChoose_triggered();

        void on_dateChanged();

        void on_actionVariableNone_triggered();

        void on_rasterScaleButton_clicked();

        void on_variableButton_clicked();

        void on_frequencyButton_clicked();

        void enableAllDataset(bool toggled);

        void disableAllButton(bool toggled);

        void on_actionVariableQualitySpatial_triggered();

        void on_actionPointsVisible_triggered();

        void on_action_Open_NetCDF_data_triggered();

        void on_action_Extract_NetCDF_series_triggered();

        void on_rasterRestoreButton_clicked();

        void on_timeEdit_timeChanged(const QTime &time);

        void on_dateEdit_dateChanged(const QDate &date);

        void on_actionClose_meteo_points_triggered();

        void on_actionClose_meteo_grid_triggered();

        void on_actionInterpolation_to_DTM_triggered();

        void on_actionInterpolation_to_Grid_triggered();

        void on_actionSave_meteo_grid_triggered();

        void on_actionOpen_model_parameters_triggered();

        void on_actionOpen_soil_map_triggered();

        void on_actionOpen_soil_data_triggered();

        void on_actionInitializeCriteria3D_triggered();

        void on_actionShow_boundary_triggered();

        void on_actionShow_soil_triggered();

        void on_actionShow_DTM_triggered();

        void on_actionElaboration_meteo_grid_triggered();

        void on_actionElaboration_meteo_points_triggered();

        void on_actionAnomaly_meteo_points_triggered();

        void on_actionAnomaly_meteo_grid_triggered();

        void on_actionClimate_meteo_points_triggered();

        void on_actionClimate_meteo_grid_triggered();

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

        MapGraphicsScene* mapScene;
        MapGraphicsView* mapView;
        RasterObject* rasterObj;
        RasterObject* meteoGridObj;
        ColorLegend *rasterLegend;
        ColorLegend *meteoPointsLegend;
        ColorLegend *meteoGridLegend;
        QList<StationMarker*> pointList;
        RubberBand *myRubberBand;
        bool showPoints;

        environment menu;
        QList<QCheckBox*> datasetCheckbox;
        QCheckBox* all;

        QGroupBox* elaborationBox;
        QLineEdit* elabType1;
        QLineEdit* elabType2;
        QLineEdit* elabVariable;
        QLineEdit* elabPeriod;

        void setMapSource(OSMTileSource::OSMTileType mySource);
        QString selectArkimetDataset(QDialog* datasetDialog);

        QPoint getMapPoint(QPoint* point) const;

        void updateVariable();
        void updateDateTime();
        void resetMeteoPoints();
        void addMeteoPoints();
        void redrawMeteoPoints(bool updateColorSCale);
        void redrawMeteoGrid();

        bool loadMeteoPointsDB(QString dbName);
        bool loadMeteoGridDB(QString xmlName);
        void setCurrentRaster(gis::Crit3DRasterGrid *myRaster);
        void interpolateRasterGUI();
        void showElabResult(bool updateColorSCale);

    };

    void exportNetCDFDataSeries(gis::Crit3DGeoPoint geoPoint);


#endif // MAINWINDOW_H
