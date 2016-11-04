#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>
    #include <QGridLayout>
    #include "MapGraphicsView.h"
    #include "MapGraphicsScene.h"

    namespace Ui {
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

    private:
        Ui::MainWindow* ui;
        MapGraphicsScene* scene;
        MapGraphicsView * view;

    };

#endif // MAINWINDOW_H

