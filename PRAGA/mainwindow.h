#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QGridLayout>
    #include <QList>
    #include <QCheckBox>
    #include <QCalendarWidget>
    #include <QDateEdit>
    #include <QLabel>
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

        void on_actionMeteoPointsArkimet_triggered();

        void on_actionOpen_meteo_points_DB_triggered();

        void on_actionDownload_meteo_data_triggered();

        void on_opacitySlider_sliderMoved(int position);

        void on_actionMapToner_triggered();

        void on_actionMapOpenStreetMap_triggered();

        void on_actionMapWikimedia_triggered();

        void on_actionMapTerrain_triggered();

        void on_actionSetUTMzone_triggered();

        void on_actionRectangle_Selection_triggered();

        void setCalendarDate(const QDate& date);

        void on_actionVariableChoose_triggered();

        void on_actionVariableHourly_triggered();

        void on_actionVariableDaily_triggered();

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
        QList<StationMarker*> pointList;
        ColorLegend* legend;
        environment menu;
        RubberBand *myRubberBand;

        QList<QCheckBox*> datasetCheckbox;
        QCalendarWidget* calendar;
        QDateEdit* FirstDateEdit;
        QLabel* FirstDateLabel;
        QDateEdit* LastDateEdit;
        QLabel* LastDateLabel;
        bool isFirstDate;
        QDate firstDate, lastDate;

        void setMapSource(OSMTileSource::OSMTileType mySource);
        QString selectArkimetDataset(QDialog* datasetDialog);

        QPoint getMapPoint(QPoint* point) const;

        void updateVariable();
        void resetMeteoPoints();
        void displayMeteoPoints();
        void enableAllDataset(bool toggled);

        void loadMeteoPointsData(DbMeteoPoints* myDB);

    };


#endif // MAINWINDOW_H
