/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoadRaster;
    QAction *actionSetUTMzone;
    QAction *actionProxy;
    QAction *actionMapOpenStreetMap;
    QAction *actionMapWikimedia;
    QAction *actionMapTerrain;
    QAction *actionMapToner;
    QAction *actionNew_meteo_points_DB;
    QAction *actionOpen_meteo_points_DB;
    QAction *actionDownload_meteo_data;
    QWidget *centralWidget;
    QWidget *widgetMap;
    QGroupBox *groupBoxRaster;
    QSlider *opacitySlider;
    QWidget *widgetColorLegend;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuMap_server;
    QMenu *menuGIS_settings;
    QMenu *menuGIS;
    QMenu *menuMap;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1024, 768);
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
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(10);
        font.setBold(false);
        font.setWeight(50);
        MainWindow->setFont(font);
        MainWindow->setAutoFillBackground(false);
        MainWindow->setAnimated(false);
        actionLoadRaster = new QAction(MainWindow);
        actionLoadRaster->setObjectName(QStringLiteral("actionLoadRaster"));
        actionSetUTMzone = new QAction(MainWindow);
        actionSetUTMzone->setObjectName(QStringLiteral("actionSetUTMzone"));
        actionProxy = new QAction(MainWindow);
        actionProxy->setObjectName(QStringLiteral("actionProxy"));
        actionMapOpenStreetMap = new QAction(MainWindow);
        actionMapOpenStreetMap->setObjectName(QStringLiteral("actionMapOpenStreetMap"));
        actionMapWikimedia = new QAction(MainWindow);
        actionMapWikimedia->setObjectName(QStringLiteral("actionMapWikimedia"));
        actionMapTerrain = new QAction(MainWindow);
        actionMapTerrain->setObjectName(QStringLiteral("actionMapTerrain"));
        actionMapToner = new QAction(MainWindow);
        actionMapToner->setObjectName(QStringLiteral("actionMapToner"));
        actionNew_meteo_points_DB = new QAction(MainWindow);
        actionNew_meteo_points_DB->setObjectName(QStringLiteral("actionNew_meteo_points_DB"));
        actionOpen_meteo_points_DB = new QAction(MainWindow);
        actionOpen_meteo_points_DB->setObjectName(QStringLiteral("actionOpen_meteo_points_DB"));
        actionDownload_meteo_data = new QAction(MainWindow);
        actionDownload_meteo_data->setObjectName(QStringLiteral("actionDownload_meteo_data"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setMinimumSize(QSize(0, 0));
        centralWidget->setMaximumSize(QSize(2000, 2000));
        widgetMap = new QWidget(centralWidget);
        widgetMap->setObjectName(QStringLiteral("widgetMap"));
        widgetMap->setGeometry(QRect(220, 0, 731, 561));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        widgetMap->setFont(font1);
        widgetMap->setMouseTracking(true);
        groupBoxRaster = new QGroupBox(centralWidget);
        groupBoxRaster->setObjectName(QStringLiteral("groupBoxRaster"));
        groupBoxRaster->setGeometry(QRect(0, 420, 211, 141));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Dark, brush2);
        palette1.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush);
        palette1.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        groupBoxRaster->setPalette(palette1);
        QFont font2;
        font2.setPointSize(10);
        groupBoxRaster->setFont(font2);
        groupBoxRaster->setAutoFillBackground(false);
        groupBoxRaster->setTitle(QStringLiteral("Raster"));
        groupBoxRaster->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        opacitySlider = new QSlider(groupBoxRaster);
        opacitySlider->setObjectName(QStringLiteral("opacitySlider"));
        opacitySlider->setGeometry(QRect(20, 30, 171, 22));
        opacitySlider->setToolTipDuration(-1);
#ifndef QT_NO_STATUSTIP
        opacitySlider->setStatusTip(QStringLiteral(""));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        opacitySlider->setWhatsThis(QStringLiteral(""));
#endif // QT_NO_WHATSTHIS
        opacitySlider->setMaximum(100);
        opacitySlider->setValue(66);
        opacitySlider->setOrientation(Qt::Horizontal);
        widgetColorLegend = new QWidget(groupBoxRaster);
        widgetColorLegend->setObjectName(QStringLiteral("widgetColorLegend"));
        widgetColorLegend->setGeometry(QRect(-1, 70, 211, 50));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 30));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush5(QColor(170, 170, 255, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush5);
        palette2.setBrush(QPalette::Active, QPalette::Light, brush1);
        QBrush brush6(QColor(212, 212, 255, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        QBrush brush7(QColor(85, 85, 127, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Dark, brush7);
        QBrush brush8(QColor(113, 113, 170, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Mid, brush8);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush);
        palette2.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush5);
        palette2.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Active, QPalette::AlternateBase, brush6);
        palette2.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette2.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush5);
        palette2.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette2.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette2.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush5);
        palette2.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush6);
        palette2.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush5);
        palette2.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette2.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush5);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush5);
        palette2.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush5);
        palette2.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        menuBar->setPalette(palette2);
        menuBar->setAutoFillBackground(false);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuMap_server = new QMenu(menuBar);
        menuMap_server->setObjectName(QStringLiteral("menuMap_server"));
        menuGIS_settings = new QMenu(menuBar);
        menuGIS_settings->setObjectName(QStringLiteral("menuGIS_settings"));
        menuGIS = new QMenu(menuGIS_settings);
        menuGIS->setObjectName(QStringLiteral("menuGIS"));
        menuMap = new QMenu(menuGIS_settings);
        menuMap->setObjectName(QStringLiteral("menuMap"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuMap_server->menuAction());
        menuBar->addAction(menuGIS_settings->menuAction());
        menuFile->addAction(actionLoadRaster);
        menuFile->addAction(actionNew_meteo_points_DB);
        menuFile->addAction(actionOpen_meteo_points_DB);
        menuFile->addAction(actionDownload_meteo_data);
        menuGIS_settings->addAction(menuGIS->menuAction());
        menuGIS_settings->addAction(menuMap->menuAction());
        menuGIS_settings->addAction(actionProxy);
        menuGIS->addAction(actionSetUTMzone);
        menuMap->addAction(actionMapOpenStreetMap);
        menuMap->addAction(actionMapWikimedia);
        menuMap->addAction(actionMapTerrain);
        menuMap->addAction(actionMapToner);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Criteria3D", Q_NULLPTR));
        actionLoadRaster->setText(QApplication::translate("MainWindow", "&Load Raster", Q_NULLPTR));
        actionSetUTMzone->setText(QApplication::translate("MainWindow", "&UTM zone", Q_NULLPTR));
        actionProxy->setText(QApplication::translate("MainWindow", "&Proxy", Q_NULLPTR));
        actionMapOpenStreetMap->setText(QApplication::translate("MainWindow", "&OpenStreetMap", Q_NULLPTR));
        actionMapWikimedia->setText(QApplication::translate("MainWindow", "&Wikimedia", Q_NULLPTR));
        actionMapTerrain->setText(QApplication::translate("MainWindow", "&Terrain", Q_NULLPTR));
        actionMapToner->setText(QApplication::translate("MainWindow", "To&ner", Q_NULLPTR));
        actionNew_meteo_points_DB->setText(QApplication::translate("MainWindow", "New meteo points DB", Q_NULLPTR));
        actionOpen_meteo_points_DB->setText(QApplication::translate("MainWindow", "Open meteo points DB", Q_NULLPTR));
        actionDownload_meteo_data->setText(QApplication::translate("MainWindow", "Download meteo data", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        opacitySlider->setToolTip(QApplication::translate("MainWindow", "Raster opacity", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_ACCESSIBILITY
        opacitySlider->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        opacitySlider->setAccessibleDescription(QString());
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_TOOLTIP
        widgetColorLegend->setToolTip(QApplication::translate("MainWindow", "Raster colors", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        menuFile->setTitle(QApplication::translate("MainWindow", "Fi&le", Q_NULLPTR));
        menuMap_server->setTitle(QApplication::translate("MainWindow", "Edit", Q_NULLPTR));
        menuGIS_settings->setTitle(QApplication::translate("MainWindow", "Settin&gs", Q_NULLPTR));
        menuGIS->setTitle(QApplication::translate("MainWindow", "&GIS", Q_NULLPTR));
        menuMap->setTitle(QApplication::translate("MainWindow", "&Map", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
