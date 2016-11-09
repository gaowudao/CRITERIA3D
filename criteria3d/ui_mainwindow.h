/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *menuFile_load_dtm;
    QAction *menuDB_Load_Points_Postgres;
    QAction *menuDB_Load_data;
    QAction *actionView_data;
    QAction *actionDateTime;
    QAction *menuInterpolationAirTemperature;
    QAction *actionLoad_soil;
    QAction *menuInterpolationSolarIrradiance;
    QAction *actionLoad_Project;
    QAction *actionInitialize_waterbalance;
    QAction *menuInterpolationPrecipitation;
    QAction *menuInterpolationWindIntensity;
    QAction *menuInterpolationAirHumidity;
    QAction *actionTry_Model_cycle;
    QAction *menuInterpolationLeafWetness;
    QAction *actionClose_Project;
    QWidget *centralWidget;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuDB;
    QMenu *menuModels;
    QMenu *menuInterpolation;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(709, 533);
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        QBrush brush2(QColor(127, 127, 127, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush2);
        QBrush brush3(QColor(170, 170, 170, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        QBrush brush4(QColor(255, 255, 220, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        MainWindow->setPalette(palette);
        MainWindow->setAnimated(false);
        menuFile_load_dtm = new QAction(MainWindow);
        menuFile_load_dtm->setObjectName(QStringLiteral("menuFile_load_dtm"));
        menuDB_Load_Points_Postgres = new QAction(MainWindow);
        menuDB_Load_Points_Postgres->setObjectName(QStringLiteral("menuDB_Load_Points_Postgres"));
        menuDB_Load_data = new QAction(MainWindow);
        menuDB_Load_data->setObjectName(QStringLiteral("menuDB_Load_data"));
        actionView_data = new QAction(MainWindow);
        actionView_data->setObjectName(QStringLiteral("actionView_data"));
        actionDateTime = new QAction(MainWindow);
        actionDateTime->setObjectName(QStringLiteral("actionDateTime"));
        menuInterpolationAirTemperature = new QAction(MainWindow);
        menuInterpolationAirTemperature->setObjectName(QStringLiteral("menuInterpolationAirTemperature"));
        actionLoad_soil = new QAction(MainWindow);
        actionLoad_soil->setObjectName(QStringLiteral("actionLoad_soil"));
        menuInterpolationSolarIrradiance = new QAction(MainWindow);
        menuInterpolationSolarIrradiance->setObjectName(QStringLiteral("menuInterpolationSolarIrradiance"));
        actionLoad_Project = new QAction(MainWindow);
        actionLoad_Project->setObjectName(QStringLiteral("actionLoad_Project"));
        actionInitialize_waterbalance = new QAction(MainWindow);
        actionInitialize_waterbalance->setObjectName(QStringLiteral("actionInitialize_waterbalance"));
        menuInterpolationPrecipitation = new QAction(MainWindow);
        menuInterpolationPrecipitation->setObjectName(QStringLiteral("menuInterpolationPrecipitation"));
        menuInterpolationWindIntensity = new QAction(MainWindow);
        menuInterpolationWindIntensity->setObjectName(QStringLiteral("menuInterpolationWindIntensity"));
        menuInterpolationAirHumidity = new QAction(MainWindow);
        menuInterpolationAirHumidity->setObjectName(QStringLiteral("menuInterpolationAirHumidity"));
        actionTry_Model_cycle = new QAction(MainWindow);
        actionTry_Model_cycle->setObjectName(QStringLiteral("actionTry_Model_cycle"));
        menuInterpolationLeafWetness = new QAction(MainWindow);
        menuInterpolationLeafWetness->setObjectName(QStringLiteral("menuInterpolationLeafWetness"));
        actionClose_Project = new QAction(MainWindow);
        actionClose_Project->setObjectName(QStringLiteral("actionClose_Project"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush5(QColor(179, 0, 2, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush5);
        QBrush brush6(QColor(255, 13, 16, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Light, brush6);
        QBrush brush7(QColor(217, 6, 9, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Midlight, brush7);
        QBrush brush8(QColor(89, 0, 1, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Dark, brush8);
        QBrush brush9(QColor(119, 0, 1, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Mid, brush9);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush);
        palette1.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush5);
        palette1.setBrush(QPalette::Active, QPalette::Shadow, brush);
        QBrush brush10(QColor(217, 127, 128, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush5);
        palette1.setBrush(QPalette::Inactive, QPalette::Light, brush6);
        palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush7);
        palette1.setBrush(QPalette::Inactive, QPalette::Dark, brush8);
        palette1.setBrush(QPalette::Inactive, QPalette::Mid, brush9);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush5);
        palette1.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush8);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush5);
        palette1.setBrush(QPalette::Disabled, QPalette::Light, brush6);
        palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush7);
        palette1.setBrush(QPalette::Disabled, QPalette::Dark, brush8);
        palette1.setBrush(QPalette::Disabled, QPalette::Mid, brush9);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush8);
        palette1.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush8);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush5);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush5);
        palette1.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush5);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        statusBar->setPalette(palette1);
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
        font.setBold(false);
        font.setWeight(50);
        statusBar->setFont(font);
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 709, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuDB = new QMenu(menuBar);
        menuDB->setObjectName(QStringLiteral("menuDB"));
        menuModels = new QMenu(menuBar);
        menuModels->setObjectName(QStringLiteral("menuModels"));
        menuInterpolation = new QMenu(menuBar);
        menuInterpolation->setObjectName(QStringLiteral("menuInterpolation"));
        MainWindow->setMenuBar(menuBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        toolBar->setMinimumSize(QSize(0, 20));
        toolBar->setMovable(false);
        toolBar->setAllowedAreas(Qt::TopToolBarArea);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuDB->menuAction());
        menuBar->addAction(menuModels->menuAction());
        menuBar->addAction(menuInterpolation->menuAction());
        menuFile->addAction(actionLoad_Project);
        menuFile->addAction(menuFile_load_dtm);
        menuFile->addSeparator();
        menuFile->addAction(actionClose_Project);
        menuDB->addAction(menuDB_Load_Points_Postgres);
        menuDB->addAction(menuDB_Load_data);
        menuDB->addAction(actionLoad_soil);
        menuModels->addAction(actionInitialize_waterbalance);
        menuModels->addAction(actionTry_Model_cycle);
        menuInterpolation->addAction(menuInterpolationAirTemperature);
        menuInterpolation->addAction(menuInterpolationPrecipitation);
        menuInterpolation->addAction(menuInterpolationAirHumidity);
        menuInterpolation->addAction(menuInterpolationSolarIrradiance);
        menuInterpolation->addAction(menuInterpolationWindIntensity);
        menuInterpolation->addAction(menuInterpolationLeafWetness);
        toolBar->addSeparator();
        toolBar->addAction(actionLoad_Project);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Criteria3D", 0));
        menuFile_load_dtm->setText(QApplication::translate("MainWindow", "Load DTM", 0));
        menuDB_Load_Points_Postgres->setText(QApplication::translate("MainWindow", "Load Points", 0));
        menuDB_Load_data->setText(QApplication::translate("MainWindow", "Load data", 0));
        actionView_data->setText(QApplication::translate("MainWindow", "view data", 0));
        actionDateTime->setText(QApplication::translate("MainWindow", "dateTime", 0));
        menuInterpolationAirTemperature->setText(QApplication::translate("MainWindow", "Air temperature", 0));
        actionLoad_soil->setText(QApplication::translate("MainWindow", "Load soil parameters", 0));
        menuInterpolationSolarIrradiance->setText(QApplication::translate("MainWindow", "Solar irradiance", 0));
        actionLoad_Project->setText(QApplication::translate("MainWindow", "Load Project", 0));
        actionInitialize_waterbalance->setText(QApplication::translate("MainWindow", "Initialize waterbalance", 0));
        menuInterpolationPrecipitation->setText(QApplication::translate("MainWindow", "Precipitation", 0));
        menuInterpolationWindIntensity->setText(QApplication::translate("MainWindow", "Wind intensity", 0));
        menuInterpolationAirHumidity->setText(QApplication::translate("MainWindow", "Air humidity", 0));
        actionTry_Model_cycle->setText(QApplication::translate("MainWindow", "Run models", 0));
        menuInterpolationLeafWetness->setText(QApplication::translate("MainWindow", "Leaf wetness", 0));
        actionClose_Project->setText(QApplication::translate("MainWindow", "Close Project", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuDB->setTitle(QApplication::translate("MainWindow", "DB", 0));
        menuModels->setTitle(QApplication::translate("MainWindow", "Models", 0));
        menuInterpolation->setTitle(QApplication::translate("MainWindow", "Interpolation", 0));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
