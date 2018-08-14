#ifndef COMPUTATIONDIALOG_H
#define COMPUTATIONDIALOG_H

#include <QString>
#include <QSettings>
#include <QGridLayout>
#include <QComboBox>

#include <QtWidgets>
#include "project.h"
#include "mainwindow.h"

class ComputationDialog : public QDialog
{

    Q_OBJECT

    private:
        bool isAnomaly;
        bool isMeteoGrid;
        MainWindow *w;
        QString title;
        QSettings* settings;
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

    public:
        ComputationDialog(QSettings *settings, bool isAnomaly, bool isMeteoGrid, MainWindow *w);
        //bool computation(bool isAnomaly);
        //bool computation(bool isAnomaly, bool isMeteoGrid, MainWindow* w);

//        QString getTitle() const;
//        void setTitle(const QString &value);

//        QSettings *getSettings() const;
//        void setSettings(QSettings *value);

        void done(bool res);
        void displayPeriod(const QString value);
        void checkYears();
        void changeDate(const QDate newDate);
        void listElaboration(const QString value);
        void listSecondElab(const QString value);
        void activeSecondParameter(const QString value);
        void readParameter(int state);


//        bool getIsAnomaly() const;
//        void setIsAnomaly(bool value);
//        bool getIsMeteoGrid() const;
//        void setIsMeteoGrid(bool value);
//        MainWindow *getW() const;
//        void setW(MainWindow *value);
    signals:
        void finished();
};


#endif // COMPUTATIONDIALOG_H
