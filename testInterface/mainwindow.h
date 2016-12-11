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
        /*!
         * \brief on_actionLoad_Raster_triggered load Raster
         */
        void on_actionLoad_Raster_triggered();

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

    private:
        Ui::MainWindow* ui;
        MapGraphicsScene* scene;
        MapGraphicsView* view;
        RasterObject* rasterMap;
    };


#endif // MAINWINDOW_H
