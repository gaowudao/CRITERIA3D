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
        QString variableElab;

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
        void AnomalyListSecondElab(const QString value);
        void AnomalyActiveSecondParameter(const QString value);
        void AnomalyReadParameter(int state);
        QString AnomalyGetVariableElab() const;
        void AnomalySetVariableElab(const QString &value);

};


#endif // ANOMALYLAYOUT_H
