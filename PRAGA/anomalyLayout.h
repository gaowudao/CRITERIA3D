#ifndef ANOMALYLAYOUT_H
#define ANOMALYLAYOUT_H

#include <QString>
#include <QSettings>
#include <QGridLayout>
#include <QComboBox>

#include <QtWidgets>
#include "pragaProject.h"
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
        QHBoxLayout readParamLayout;
        QHBoxLayout secondElabLayout;

        QStringList climateDbElab;
        QComboBox climateDbElabList;

    public:
        AnomalyLayout();
        void build(QSettings *settings);
        void AnomalyDisplayPeriod(const QString value);
        void AnomalyCheckYears();
        void AnomalyListElaboration(const QString value);
        void AnomalyListSecondElab(const QString value);
        void AnomalyActiveSecondParameter(const QString value);
        void AnomalyReadParameter(int state);

        void AnomalySetVariableElab(const QString &value);
        QString AnomalyGetPeriodTypeList() const;
        void AnomalySetPeriodTypeList(QString period);
        void AnomalySetReadReference(bool set);
        int AnomalyGetYearStart() const;
        int AnomalyGetYearLast() const;
        void AnomalySetYearStart(QString year);
        void AnomalySetYearLast(QString year);
        QDate AnomalyGetGenericPeriodStart() const;
        void AnomalySetGenericPeriodStart(QDate genericStart);
        QDate AnomalyGetGenericPeriodEnd() const;
        void AnomalySetGenericPeriodEnd(QDate genericEnd);
        int AnomalyGetNyears() const;
        void AnomalySetNyears(QString nYears);
        QDate AnomalyGetCurrentDay() const;
        void AnomalySetCurrentDay(QDate date);
        QString AnomalyGetElaboration() const;
        void AnomalySetElaboration(QString elab);
        QString AnomalyGetSecondElaboration() const;
        void AnomalySetSecondElaboration(QString elab);
        QString AnomalyGetParam1() const;
        void AnomalySetParam1(QString param);
        QString AnomalyGetParam2() const;
        void AnomalySetParam2(QString param);
        bool AnomalyReadParamIsChecked() const;
        void AnomalySetReadParamIsChecked(bool set);
};


#endif // ANOMALYLAYOUT_H
