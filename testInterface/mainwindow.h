#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QGridLayout>
    #include "MapGraphicsView.h"
    #include "MapGraphicsScene.h"
    #include "Position.h"

    class RasterObject;

    namespace Ui
    {
        class MainWindow;
    }

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void on_actionLoad_Raster_triggered();

    protected:
        void mouseReleaseEvent(QMouseEvent *event);

    private:
        Ui::MainWindow* ui;
        MapGraphicsScene* scene;
        MapGraphicsView* view;
        RasterObject* rasterMap;
        Position* startCenter;
    };


#endif // MAINWINDOW_H
