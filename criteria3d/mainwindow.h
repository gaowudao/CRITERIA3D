#ifndef MAINWINDOW_H
#define MAINWINDOW_H

    #include <QMainWindow>

    #ifndef QSTRING_H
        #include <QString>
    #endif
    #ifndef MAPWIDGET_H
        #include "mapWidget.h"
    #endif
    #ifndef METEO_H
        #include "meteo.h"
    #endif

    namespace Ui {
        class MainWindow;
        }

        class MainWindow : public QMainWindow
        {
            Q_OBJECT

        public:
            explicit MainWindow(QWidget *parent = 0);
            mapWidget myMapWidget;
            ~MainWindow();

        private slots:

            void on_menuDB_Load_data_triggered();
            void on_menuDB_Load_Points_Postgres_triggered();
            void on_menuFile_load_dtm_triggered();
            void on_actionLoad_soil_triggered();

            void on_menuInterpolationSolarIrradiance_triggered();
            void on_menuInterpolationAirTemperature_triggered();
            void on_menuInterpolationPrecipitation_triggered();
            void on_menuInterpolationWindIntensity_triggered();
            void on_menuInterpolationAirHumidity_triggered();
            void on_menuInterpolationLeafWetness_triggered();

            bool interpolateMap(meteoVariable myVar);

            void on_actionLoad_Project_triggered();

            void on_actionInitialize_waterbalance_triggered();

            void on_actionTry_Model_cycle_triggered();

            void on_actionClose_Project_triggered();

        private:

            Ui::MainWindow *ui;

        };

#endif // MAINWINDOW_H
