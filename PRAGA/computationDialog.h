#ifndef COMPUTATIONDIALOG_H
#define COMPUTATIONDIALOG_H

#include <QString>
#include <QSettings>
#include <QGridLayout>
#include <QComboBox>

#include <QtWidgets>
#include "pragaProject.h"
#include "mainWindow.h"
#include "anomalyLayout.h"
#include "saveClimaLayout.h"


class ComputationDialog : public QDialog
{

    Q_OBJECT

    private:
        QSettings* settings;
        bool isAnomaly;
        bool saveClima;
        QString title;
        QDateEdit currentDay;
        QLabel currentDayLabel;
        QComboBox variableList;
        QLineEdit firstYearEdit;
        QLineEdit lastYearEdit;
        QLabel genericStartLabel;
        QLabel genericEndLabel;
        QLabel nrYearLabel;
        QDateEdit genericPeriodStart;
        QDateEdit genericPeriodEnd;
        QLineEdit nrYear;
        QCheckBox readParam;

        QComboBox periodTypeList;
        QComboBox elaborationList;
        QComboBox secondElabList;
        QLineEdit periodDisplay;

        QLineEdit elab1Parameter;
        QLineEdit elab2Parameter;

        AnomalyLayout anomaly;
        QPushButton copyData;

        QPushButton add;
        QPushButton del;
        QPushButton delAll;
        SaveClimaLayout saveClimaLayout;

        QStringList climateDbElab;
        QComboBox climateDbElabList;



    public:
        ComputationDialog(QSettings *settings, bool isAnomaly, bool saveClima);
        void done(bool res);
        void displayPeriod(const QString value);
        void checkYears();
        void listElaboration(const QString value);
        void listSecondElab(const QString value);
        void activeSecondParameter(const QString value);
        void readParameter(int state);
        void copyDataToAnomaly();
        void copyDataToSaveLayout();
        bool checkValidData();
        QStringList getElabSaveList();
};


#endif // COMPUTATIONDIALOG_H
