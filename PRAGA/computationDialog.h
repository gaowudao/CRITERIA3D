#ifndef COMPUTATIONDIALOG_H
#define COMPUTATIONDIALOG_H

#include <QString>
#include <QSettings>
#include <QGridLayout>
#include <QComboBox>

#include <QtWidgets>
#include "project.h"

class ComputationDialog : public QDialog
{

    Q_OBJECT

    private:
        QString title;
        QSettings* settings;
        QDateEdit currentDay;
        QLabel currentDayLabel;
        QLineEdit firstYearEdit;
        QLineEdit lastYearEdit;
        QLabel genericStartLabel;
        QLabel genericEndLabel;
        QLabel nrYearLabel;
        QDateEdit genericPeriodStart;
        QDateEdit genericPeriodEnd;
        QLineEdit nrYear;

        QComboBox periodTypeList;
        QComboBox elaborationList;
        QComboBox secondElabList;
        QLineEdit periodDisplay;

        QLineEdit elab1Parameter;
        QLineEdit elab2Parameter;

    public:
        ComputationDialog(QWidget *parent = 0);
        bool computation(bool isAnomaly);

        QString getTitle() const;
        void setTitle(const QString &value);

        QSettings *getSettings() const;
        void setSettings(QSettings *value);

        void done(int r);
        void displayPeriod(const QString value);
        void checkYears();
        void changeDate(const QDate newDate);
        void listElaboration(const QString value);
        void listSecondElab(const QString value);
        void activeSecondParameter(const QString value);
        void readParameter(int state);


};


#endif // COMPUTATIONDIALOG_H
