#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QGridLayout>
    #include <QList>
    #include <QCheckBox>
    #include "MapGraphicsView.h"
    #include "MapGraphicsScene.h"
    #include "tileSources/OSMTileSource.h"
    #include "rasterObject.h"
    #include "colorlegend.h"
    #include "guiConfiguration.h"


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
        /*!
         * \brief on_actionLoad_Raster_triggered load Raster
         */
        void on_actionLoadRaster_triggered();

        void on_actionArkimet_triggered();

        QString on_actionArkimet_Dataset(QDialog* datasetDialog);

        void on_actionOpen_meteo_points_DB_triggered();

        void on_actionDownload_meteo_data_triggered();

        void on_opacitySlider_sliderMoved(int position);

        void on_actionMapToner_triggered();

        void on_actionMapOpenStreetMap_triggered();

        void on_actionMapWikimedia_triggered();

        void on_actionMapTerrain_triggered();

        void on_actionSetUTMzone_triggered();

        void enableAll(bool toggled);



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

        void resizeEvent(QResizeEvent * event);

    private:
        Ui::MainWindow* ui;
        MapGraphicsScene* mapScene;
        MapGraphicsView* mapView;
        RasterObject* rasterObj;
        ColorLegend* legend;
        environment menu;

        QList<QCheckBox*> datasetCheckbox;

        void setMapSource(OSMTileSource::OSMTileType mySource);

        QPoint getMapPoint(QPoint* point) const;

    };


#endif // MAINWINDOW_H
