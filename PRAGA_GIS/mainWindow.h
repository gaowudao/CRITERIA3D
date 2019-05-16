#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QtWidgets>

    #include "tileSources/OSMTileSource.h"
    #include "Position.h"

    #include "rasterObject.h"
    #include "gisProject.h"
    #include "colorlegend.h"
    #include "showProperties.h"

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

        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:

        void on_actionMapOpenStreetMap_triggered();
        void on_actionMapESRISatellite_triggered();
        void on_actionMapTerrain_triggered();

        void on_actionLoadRaster_triggered();
        void on_actionLoadShapefile_triggered();

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
        std::vector<RasterObject *> rasterObjList;
        std::vector<ColorLegend *> rasterColorScaleList;

        void updateCenter();
        void setMapSource(OSMTileSource::OSMTileType mySource);
        void addRasterObject(GisObject* myObject);
        void addShapeObject(GisObject* myObject);
        void itemClicked(QListWidgetItem* item);
        void itemMenuRequested(const QPoint point);

        QPoint getMapPoint(QPoint* point) const;
    };


#endif // MAINWINDOW_H
