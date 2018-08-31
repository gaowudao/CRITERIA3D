#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QList>
    #include <QCheckBox>
    #include <QGroupBox>

    #include "..\PRAGA\rubberBand.h"
    #include "MapGraphicsView.h"
    #include "MapGraphicsScene.h"
    #include "tileSources/OSMTileSource.h"
    #include "..\PRAGA\rasterObject.h"
    #include "..\PRAGA\stationMarker.h"
    #include "..\PRAGA\colorlegend.h"
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

        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();


    private slots:

        void on_actionLoadDEM_triggered();

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

        void on_actionShow_boundary_triggered();

        void on_actionShow_soil_triggered();

        void on_actionShow_DTM_triggered();

        void on_actionInterpolationSettings_triggered();

        void on_actionCriteria3D_settings_triggered();

        void on_actionCriteria3D_Initialize_triggered();

        void on_meteoGridOpacitySlider_sliderMoved(int position);

    #ifdef NETCDF
        void on_actionOpen_NetCDF_data_triggered();
        void on_actionExtract_NetCDF_series_triggered();
    #endif

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
        void interpolateGridGUI();
        void showElabResult(bool updateColorSCale, bool isMeteoGrid, bool isAnomaly);
    };


    #ifdef NETCDF
        void exportNetCDFDataSeries(gis::Crit3DGeoPoint geoPoint);
    #endif


#endif // MAINWINDOW_H
