#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QtWidgets>

    #include "tileSources/OSMTileSource.h"
    #include "Position.h"

    #include "mapGraphicsRasterObject.h"
    #include "mapGraphicsShapeObject.h"
    #include "gisProject.h"
    #include "colorLegend.h"
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
        void on_actionRasterize_shape_triggered();
        void on_actionCompute_Unit_Crop_Map_triggered();

        void updateMaps();

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
        std::vector<MapGraphicsShapeObject *> shapeObjList;

        void setMapSource(OSMTileSource::OSMTileType mySource);
        void addRasterObject(GisObject* myObject);
        bool addShapeObject(GisObject* myObject, QString referenceField);
        void itemClicked(QListWidgetItem* item);
        void itemMenuRequested(const QPoint point);
        void saveRaster(GisObject* myObject);
        void removeRaster(GisObject* myObject);
        void removeShape(GisObject* myObject);

        QPoint getMapPos(const QPoint& pos);
        bool isInsideMap(const QPoint& pos);
    };


#endif // MAINWINDOW_H
