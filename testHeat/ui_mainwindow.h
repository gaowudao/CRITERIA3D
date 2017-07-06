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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *formLayoutWidget;
    QFormLayout *layoutPlot;
    QListWidget *listWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QLabel *label_4;
    QLineEdit *lineEditMaxThickness;
    QLineEdit *lineEditWidth;
    QLabel *label_3;
    QLabel *label;
    QLineEdit *lineEditDepth;
    QLabel *label_2;
    QLineEdit *lineEditMinThickness;
    QLabel *label_19;
    QLabel *label_18;
    QLabel *label_14;
    QLabel *label_15;
    QCheckBox *chkBoxGeometricProgression;
    QWidget *tabBoundary;
    QGroupBox *groupBox_3;
    QLineEdit *lineEditAtmT;
    QLabel *label_9;
    QLabel *label_10;
    QLineEdit *lineEditAtmRH;
    QLineEdit *lineEditAtmWS;
    QLabel *label_11;
    QLineEdit *lineEditAtmGR;
    QLabel *label_27;
    QGroupBox *groupBox_9;
    QLineEdit *lineEditBottomT;
    QLabel *label_25;
    QGroupBox *groupBox;
    QLineEdit *lineEditPrecStart;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *lineEditPrecHours;
    QLineEdit *lineEditPrecHourlyAmount;
    QLabel *label_7;
    QWidget *tab_2;
    QLineEdit *lineEditRoughnessHeight;
    QLineEdit *lineEditPlough;
    QLineEdit *lineEditAlbedo;
    QLabel *label_21;
    QLineEdit *lineEditManningRoughness;
    QLabel *label_24;
    QLabel *label_23;
    QLabel *label_22;
    QWidget *tab_3;
    QGroupBox *groupBox_10;
    QLineEdit *lineEditClay;
    QLabel *label_26;
    QLabel *label_29;
    QLineEdit *lineEditOrganic;
    QLabel *label_30;
    QLabel *label_31;
    QLineEdit *lineEditThetaS;
    QLineEdit *lineEditThetaR;
    QWidget *tab_4;
    QGroupBox *groupBox_5;
    QLineEdit *lineEditIniWaterContent;
    QLineEdit *lineEditIniTTop;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_20;
    QLineEdit *lineEditIniTBottom;
    QWidget *tab_5;
    QGroupBox *groupBox_6;
    QCheckBox *chkBoxWater;
    QCheckBox *chkBoxHeat;
    QPushButton *pushLoadFileMeteo;
    QPushButton *pushLoadFileSoil;
    QCheckBox *chkUseInput;
    QGroupBox *groupBox_4;
    QLineEdit *lineEditSimStart;
    QLabel *label_12;
    QLabel *label_13;
    QLineEdit *lineEditSimStop;
    QPushButton *pushInitialize;
    QPushButton *pushRunNextHour;
    QPushButton *pushRunAllPeriod;
    QProgressBar *prgBar;
    QLabel *labelInfo;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1131, 563);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        formLayoutWidget = new QWidget(centralWidget);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(420, 30, 691, 471));
        layoutPlot = new QFormLayout(formLayoutWidget);
        layoutPlot->setSpacing(6);
        layoutPlot->setContentsMargins(11, 11, 11, 11);
        layoutPlot->setObjectName(QStringLiteral("layoutPlot"));
        layoutPlot->setContentsMargins(0, 0, 0, 0);
        listWidget = new QListWidget(centralWidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(10, 330, 401, 171));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 401, 311));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        label_4 = new QLabel(tab);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 50, 71, 16));
        lineEditMaxThickness = new QLineEdit(tab);
        lineEditMaxThickness->setObjectName(QStringLiteral("lineEditMaxThickness"));
        lineEditMaxThickness->setGeometry(QRect(100, 50, 31, 20));
        lineEditWidth = new QLineEdit(tab);
        lineEditWidth->setObjectName(QStringLiteral("lineEditWidth"));
        lineEditWidth->setGeometry(QRect(100, 70, 31, 20));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 30, 71, 16));
        label = new QLabel(tab);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 70, 61, 16));
        lineEditDepth = new QLineEdit(tab);
        lineEditDepth->setObjectName(QStringLiteral("lineEditDepth"));
        lineEditDepth->setGeometry(QRect(100, 90, 31, 20));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 90, 61, 16));
        lineEditMinThickness = new QLineEdit(tab);
        lineEditMinThickness->setObjectName(QStringLiteral("lineEditMinThickness"));
        lineEditMinThickness->setGeometry(QRect(100, 30, 31, 20));
        label_19 = new QLabel(tab);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(150, 90, 21, 16));
        label_18 = new QLabel(tab);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(150, 70, 21, 16));
        label_14 = new QLabel(tab);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(150, 30, 21, 16));
        label_15 = new QLabel(tab);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(150, 50, 21, 16));
        chkBoxGeometricProgression = new QCheckBox(tab);
        chkBoxGeometricProgression->setObjectName(QStringLiteral("chkBoxGeometricProgression"));
        chkBoxGeometricProgression->setGeometry(QRect(20, 130, 141, 17));
        tabWidget->addTab(tab, QString());
        tabBoundary = new QWidget();
        tabBoundary->setObjectName(QStringLiteral("tabBoundary"));
        groupBox_3 = new QGroupBox(tabBoundary);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 10, 231, 111));
        lineEditAtmT = new QLineEdit(groupBox_3);
        lineEditAtmT->setObjectName(QStringLiteral("lineEditAtmT"));
        lineEditAtmT->setGeometry(QRect(140, 20, 31, 20));
        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 20, 91, 16));
        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(10, 40, 101, 16));
        lineEditAtmRH = new QLineEdit(groupBox_3);
        lineEditAtmRH->setObjectName(QStringLiteral("lineEditAtmRH"));
        lineEditAtmRH->setGeometry(QRect(140, 40, 31, 20));
        lineEditAtmWS = new QLineEdit(groupBox_3);
        lineEditAtmWS->setObjectName(QStringLiteral("lineEditAtmWS"));
        lineEditAtmWS->setGeometry(QRect(140, 60, 31, 20));
        label_11 = new QLabel(groupBox_3);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(10, 60, 101, 16));
        lineEditAtmGR = new QLineEdit(groupBox_3);
        lineEditAtmGR->setObjectName(QStringLiteral("lineEditAtmGR"));
        lineEditAtmGR->setGeometry(QRect(140, 80, 31, 20));
        label_27 = new QLabel(groupBox_3);
        label_27->setObjectName(QStringLiteral("label_27"));
        label_27->setGeometry(QRect(10, 80, 121, 16));
        groupBox_9 = new QGroupBox(tabBoundary);
        groupBox_9->setObjectName(QStringLiteral("groupBox_9"));
        groupBox_9->setGeometry(QRect(20, 130, 181, 51));
        lineEditBottomT = new QLineEdit(groupBox_9);
        lineEditBottomT->setObjectName(QStringLiteral("lineEditBottomT"));
        lineEditBottomT->setGeometry(QRect(140, 20, 31, 20));
        label_25 = new QLabel(groupBox_9);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setGeometry(QRect(10, 20, 91, 16));
        groupBox = new QGroupBox(tabBoundary);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 200, 181, 81));
        lineEditPrecStart = new QLineEdit(groupBox);
        lineEditPrecStart->setObjectName(QStringLiteral("lineEditPrecStart"));
        lineEditPrecStart->setGeometry(QRect(110, 10, 51, 20));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 10, 71, 16));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 30, 71, 16));
        lineEditPrecHours = new QLineEdit(groupBox);
        lineEditPrecHours->setObjectName(QStringLiteral("lineEditPrecHours"));
        lineEditPrecHours->setGeometry(QRect(110, 30, 51, 20));
        lineEditPrecHourlyAmount = new QLineEdit(groupBox);
        lineEditPrecHourlyAmount->setObjectName(QStringLiteral("lineEditPrecHourlyAmount"));
        lineEditPrecHourlyAmount->setGeometry(QRect(110, 50, 51, 20));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(10, 50, 71, 16));
        tabWidget->addTab(tabBoundary, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        lineEditRoughnessHeight = new QLineEdit(tab_2);
        lineEditRoughnessHeight->setObjectName(QStringLiteral("lineEditRoughnessHeight"));
        lineEditRoughnessHeight->setGeometry(QRect(150, 35, 41, 20));
        lineEditPlough = new QLineEdit(tab_2);
        lineEditPlough->setObjectName(QStringLiteral("lineEditPlough"));
        lineEditPlough->setGeometry(QRect(150, 95, 41, 20));
        lineEditAlbedo = new QLineEdit(tab_2);
        lineEditAlbedo->setObjectName(QStringLiteral("lineEditAlbedo"));
        lineEditAlbedo->setGeometry(QRect(150, 15, 41, 20));
        label_21 = new QLabel(tab_2);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(30, 15, 51, 16));
        lineEditManningRoughness = new QLineEdit(tab_2);
        lineEditManningRoughness->setObjectName(QStringLiteral("lineEditManningRoughness"));
        lineEditManningRoughness->setGeometry(QRect(150, 75, 41, 20));
        label_24 = new QLabel(tab_2);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setGeometry(QRect(30, 35, 111, 21));
        label_24->setWordWrap(true);
        label_23 = new QLabel(tab_2);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(30, 95, 51, 16));
        label_22 = new QLabel(tab_2);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(30, 60, 101, 31));
        label_22->setTextFormat(Qt::PlainText);
        label_22->setScaledContents(false);
        label_22->setWordWrap(true);
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        groupBox_10 = new QGroupBox(tab_3);
        groupBox_10->setObjectName(QStringLiteral("groupBox_10"));
        groupBox_10->setGeometry(QRect(20, 10, 231, 121));
        lineEditClay = new QLineEdit(groupBox_10);
        lineEditClay->setObjectName(QStringLiteral("lineEditClay"));
        lineEditClay->setGeometry(QRect(130, 20, 41, 20));
        label_26 = new QLabel(groupBox_10);
        label_26->setObjectName(QStringLiteral("label_26"));
        label_26->setGeometry(QRect(10, 20, 61, 16));
        label_29 = new QLabel(groupBox_10);
        label_29->setObjectName(QStringLiteral("label_29"));
        label_29->setGeometry(QRect(10, 40, 111, 21));
        label_29->setWordWrap(true);
        lineEditOrganic = new QLineEdit(groupBox_10);
        lineEditOrganic->setObjectName(QStringLiteral("lineEditOrganic"));
        lineEditOrganic->setGeometry(QRect(130, 40, 41, 20));
        label_30 = new QLabel(groupBox_10);
        label_30->setObjectName(QStringLiteral("label_30"));
        label_30->setGeometry(QRect(10, 60, 111, 21));
        label_30->setWordWrap(true);
        label_31 = new QLabel(groupBox_10);
        label_31->setObjectName(QStringLiteral("label_31"));
        label_31->setGeometry(QRect(10, 80, 111, 21));
        label_31->setWordWrap(true);
        lineEditThetaS = new QLineEdit(groupBox_10);
        lineEditThetaS->setObjectName(QStringLiteral("lineEditThetaS"));
        lineEditThetaS->setGeometry(QRect(130, 60, 41, 20));
        lineEditThetaR = new QLineEdit(groupBox_10);
        lineEditThetaR->setObjectName(QStringLiteral("lineEditThetaR"));
        lineEditThetaR->setGeometry(QRect(130, 80, 41, 20));
        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        groupBox_5 = new QGroupBox(tab_4);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(40, 20, 141, 141));
        lineEditIniWaterContent = new QLineEdit(groupBox_5);
        lineEditIniWaterContent->setObjectName(QStringLiteral("lineEditIniWaterContent"));
        lineEditIniWaterContent->setGeometry(QRect(10, 30, 41, 20));
        lineEditIniTTop = new QLineEdit(groupBox_5);
        lineEditIniTTop->setObjectName(QStringLiteral("lineEditIniTTop"));
        lineEditIniTTop->setGeometry(QRect(10, 70, 51, 20));
        label_16 = new QLabel(groupBox_5);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(10, 50, 101, 16));
        label_17 = new QLabel(groupBox_5);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(10, 10, 121, 16));
        label_20 = new QLabel(groupBox_5);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(10, 90, 131, 16));
        lineEditIniTBottom = new QLineEdit(groupBox_5);
        lineEditIniTBottom->setObjectName(QStringLiteral("lineEditIniTBottom"));
        lineEditIniTBottom->setGeometry(QRect(10, 110, 51, 20));
        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        groupBox_6 = new QGroupBox(tab_5);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(20, 20, 101, 81));
        chkBoxWater = new QCheckBox(groupBox_6);
        chkBoxWater->setObjectName(QStringLiteral("chkBoxWater"));
        chkBoxWater->setEnabled(true);
        chkBoxWater->setGeometry(QRect(10, 30, 61, 17));
        chkBoxHeat = new QCheckBox(groupBox_6);
        chkBoxHeat->setObjectName(QStringLiteral("chkBoxHeat"));
        chkBoxHeat->setEnabled(true);
        chkBoxHeat->setGeometry(QRect(10, 50, 61, 17));
        chkBoxHeat->setChecked(true);
        pushLoadFileMeteo = new QPushButton(tab_5);
        pushLoadFileMeteo->setObjectName(QStringLiteral("pushLoadFileMeteo"));
        pushLoadFileMeteo->setGeometry(QRect(150, 40, 81, 23));
        pushLoadFileSoil = new QPushButton(tab_5);
        pushLoadFileSoil->setObjectName(QStringLiteral("pushLoadFileSoil"));
        pushLoadFileSoil->setGeometry(QRect(150, 70, 81, 23));
        chkUseInput = new QCheckBox(tab_5);
        chkUseInput->setObjectName(QStringLiteral("chkUseInput"));
        chkUseInput->setGeometry(QRect(150, 20, 91, 17));
        groupBox_4 = new QGroupBox(tab_5);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(20, 130, 111, 71));
        lineEditSimStart = new QLineEdit(groupBox_4);
        lineEditSimStart->setObjectName(QStringLiteral("lineEditSimStart"));
        lineEditSimStart->setGeometry(QRect(70, 20, 31, 20));
        label_12 = new QLabel(groupBox_4);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(10, 20, 91, 16));
        label_13 = new QLabel(groupBox_4);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(10, 40, 101, 16));
        lineEditSimStop = new QLineEdit(groupBox_4);
        lineEditSimStop->setObjectName(QStringLiteral("lineEditSimStop"));
        lineEditSimStop->setGeometry(QRect(70, 40, 31, 20));
        pushInitialize = new QPushButton(tab_5);
        pushInitialize->setObjectName(QStringLiteral("pushInitialize"));
        pushInitialize->setGeometry(QRect(150, 120, 81, 23));
        pushRunNextHour = new QPushButton(tab_5);
        pushRunNextHour->setObjectName(QStringLiteral("pushRunNextHour"));
        pushRunNextHour->setGeometry(QRect(150, 150, 81, 23));
        pushRunAllPeriod = new QPushButton(tab_5);
        pushRunAllPeriod->setObjectName(QStringLiteral("pushRunAllPeriod"));
        pushRunAllPeriod->setGeometry(QRect(150, 180, 81, 23));
        prgBar = new QProgressBar(tab_5);
        prgBar->setObjectName(QStringLiteral("prgBar"));
        prgBar->setGeometry(QRect(20, 220, 351, 23));
        prgBar->setValue(0);
        labelInfo = new QLabel(tab_5);
        labelInfo->setObjectName(QStringLiteral("labelInfo"));
        labelInfo->setGeometry(QRect(20, 250, 311, 16));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        labelInfo->setFont(font);
        tabWidget->addTab(tab_5, QString());
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1131, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(5);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label_4->setText(QApplication::translate("MainWindow", "max thickness", 0));
        lineEditMaxThickness->setText(QApplication::translate("MainWindow", "0.2", 0));
        lineEditWidth->setText(QApplication::translate("MainWindow", "1", 0));
        label_3->setText(QApplication::translate("MainWindow", "min thickness", 0));
        label->setText(QApplication::translate("MainWindow", "profile width", 0));
        lineEditDepth->setText(QApplication::translate("MainWindow", "0.5", 0));
        label_2->setText(QApplication::translate("MainWindow", "profile depth", 0));
        lineEditMinThickness->setText(QApplication::translate("MainWindow", "0.05", 0));
        label_19->setText(QApplication::translate("MainWindow", "m", 0));
        label_18->setText(QApplication::translate("MainWindow", "m2", 0));
        label_14->setText(QApplication::translate("MainWindow", "m", 0));
        label_15->setText(QApplication::translate("MainWindow", "m", 0));
        chkBoxGeometricProgression->setText(QApplication::translate("MainWindow", "geometric progression", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "geometry", 0));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Atmospheric boundary", 0));
        lineEditAtmT->setText(QApplication::translate("MainWindow", "20", 0));
        label_9->setText(QApplication::translate("MainWindow", "temperature (\302\260C)", 0));
        label_10->setText(QApplication::translate("MainWindow", "relative humidity (%)", 0));
        lineEditAtmRH->setText(QApplication::translate("MainWindow", "30", 0));
        lineEditAtmWS->setText(QApplication::translate("MainWindow", "5", 0));
        label_11->setText(QApplication::translate("MainWindow", "wind speed (m s-1)", 0));
        lineEditAtmGR->setText(QApplication::translate("MainWindow", "3", 0));
        label_27->setText(QApplication::translate("MainWindow", "global irradiance (W m-2)", 0));
        groupBox_9->setTitle(QApplication::translate("MainWindow", "Bottom boundary", 0));
        lineEditBottomT->setText(QApplication::translate("MainWindow", "25", 0));
        label_25->setText(QApplication::translate("MainWindow", "temperature (\302\260C)", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "Precipitation", 0));
        lineEditPrecStart->setText(QApplication::translate("MainWindow", "1000", 0));
        label_5->setText(QApplication::translate("MainWindow", "start (h)", 0));
        label_6->setText(QApplication::translate("MainWindow", "hours", 0));
        lineEditPrecHours->setText(QApplication::translate("MainWindow", "4", 0));
        lineEditPrecHourlyAmount->setText(QApplication::translate("MainWindow", "0", 0));
        label_7->setText(QApplication::translate("MainWindow", "hourly amount", 0));
        tabWidget->setTabText(tabWidget->indexOf(tabBoundary), QApplication::translate("MainWindow", "boundary", 0));
        lineEditRoughnessHeight->setText(QApplication::translate("MainWindow", "0.001", 0));
        lineEditPlough->setText(QApplication::translate("MainWindow", "0.005", 0));
        lineEditAlbedo->setText(QApplication::translate("MainWindow", "0.15", 0));
        label_21->setText(QApplication::translate("MainWindow", "albedo ()", 0));
        lineEditManningRoughness->setText(QApplication::translate("MainWindow", "0.024", 0));
        label_24->setText(QApplication::translate("MainWindow", "roughness height (m)", 0));
        label_23->setText(QApplication::translate("MainWindow", "plough (m)", 0));
        label_22->setText(QApplication::translate("MainWindow", "Manning roughness (s m-0.5)", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "surface", 0));
        groupBox_10->setTitle(QApplication::translate("MainWindow", "Soil properties", 0));
        lineEditClay->setText(QApplication::translate("MainWindow", "30", 0));
        label_26->setText(QApplication::translate("MainWindow", "clay (%)", 0));
        label_29->setText(QApplication::translate("MainWindow", "organic matter (%)", 0));
        lineEditOrganic->setText(QApplication::translate("MainWindow", "2", 0));
        label_30->setText(QApplication::translate("MainWindow", "thetaS (m3 m-3)", 0));
        label_31->setText(QApplication::translate("MainWindow", "thetaR (m3 m-3)", 0));
        lineEditThetaS->setText(QApplication::translate("MainWindow", "0.42", 0));
        lineEditThetaR->setText(QApplication::translate("MainWindow", "0.05", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "soil", 0));
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Initialization", 0));
        lineEditIniWaterContent->setText(QApplication::translate("MainWindow", "0.75", 0));
        lineEditIniTTop->setText(QApplication::translate("MainWindow", "25", 0));
        label_16->setText(QApplication::translate("MainWindow", "temperature top (\302\260C)", 0));
        label_17->setText(QApplication::translate("MainWindow", "saturation ratio ()", 0));
        label_20->setText(QApplication::translate("MainWindow", "temperature bottom  (\302\260C)", 0));
        lineEditIniTBottom->setText(QApplication::translate("MainWindow", "25", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainWindow", "initialization", 0));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "Processes", 0));
        chkBoxWater->setText(QApplication::translate("MainWindow", "water", 0));
        chkBoxHeat->setText(QApplication::translate("MainWindow", "heat", 0));
        pushLoadFileMeteo->setText(QApplication::translate("MainWindow", "Load meteo", 0));
        pushLoadFileSoil->setText(QApplication::translate("MainWindow", "Load soil", 0));
        chkUseInput->setText(QApplication::translate("MainWindow", "use input data", 0));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Simulation", 0));
        lineEditSimStart->setText(QApplication::translate("MainWindow", "0", 0));
        label_12->setText(QApplication::translate("MainWindow", "start (h)", 0));
        label_13->setText(QApplication::translate("MainWindow", "stop (h)", 0));
        lineEditSimStop->setText(QApplication::translate("MainWindow", "200", 0));
        pushInitialize->setText(QApplication::translate("MainWindow", "Initialize", 0));
        pushRunNextHour->setText(QApplication::translate("MainWindow", "Run next hour", 0));
        pushRunAllPeriod->setText(QApplication::translate("MainWindow", "Run all period", 0));
        labelInfo->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("MainWindow", "run model", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
