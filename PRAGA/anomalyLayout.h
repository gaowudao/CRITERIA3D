#ifndef ANOMALYLAYOUT_H
#define ANOMALYLAYOUT_H

#include <QString>
#include <QSettings>
#include <QGridLayout>
#include <QComboBox>

#include <QtWidgets>
#include "project.h"
#include "mainwindow.h"

class AnomalyLayout : public QDialog
{

    Q_OBJECT

    private:
        QSettings* AnomalySettings;
        QLineEdit variableElab;

        QDateEdit currentDay;
        QLabel currentDayLabel;

        QLabel firstDateLabel;
        QLineEdit firstYearEdit;

        QCheckBox readReference;

        QLabel lastDateLabel;
        QLineEdit lastYearEdit;

        QLabel periodTypeLabel;
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

        QVBoxLayout mainLayout;
        QHBoxLayout varLayout;
        QHBoxLayout dateLayout;
        QHBoxLayout periodLayout;
        QHBoxLayout displayLayout;
        QHBoxLayout genericPeriodLayout;

        QHBoxLayout elaborationLayout;
        QHBoxLayout secondElabLayout;

    public:
        AnomalyLayout();
        void build(QSettings *settings);
        void AnomalyDisplayPeriod(const QString value);
        void AnomalyCheckYears();
        void AnomalyChangeDate(const QDate newDate);
        void AnomalyListElaboration(const QString value);
        void AnomalyListSecondElab(const QString value);
        void AnomalyActiveSecondParameter(const QString value);
        void AnomalyReadParameter(int state);

        void AnomalySetVariableElab(const QString &value);
        QString AnomalyGetPeriodTypeList() const;
        int AnomalyGetYearStart() const;
        int AnomalyGetYearLast() const;
        QDate AnomalyGetGenericPeriodStart() const;
        QDate AnomalyGetGenericPeriodEnd() const;
        int AnomalyGetNyears() const;
        QDate AnomalyGetCurrentDay() const;
        QString AnomalyGetElaboration() const;
        QString AnomalyGetSecondElaboration() const;
        QString AnomalyGetParam1() const;
        QString AnomalyGetParam2() const;
        bool AnomalyReadParamIsChecked() const;
};


#endif // ANOMALYLAYOUT_H
