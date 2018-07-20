#ifndef DIALOGWINDOWS_H
#define DIALOGWINDOWS_H

    #include <QString>
    #include <QSettings>
    #include <QGridLayout>
    #include <QComboBox>

    #include <QtWidgets>

    #include "color.h"
    #include "meteo.h"

    QString editValue(QString windowsTitle, QString defaultValue);

    meteoVariable chooseColorScale();
    frequencyType chooseFrequency();

    bool chooseMeteoVariable();

    bool chooseNetCDFVariable(int *varId, QDateTime *firstDate, QDateTime *lastDate);

    bool downloadMeteoData();

    class ComputationDialog : public QDialog
    {

        Q_OBJECT

        private:
            QString title;
            QSettings* settings;
            QDateEdit currentDay;
            QLineEdit firstYearEdit;
            QLineEdit lastYearEdit;
            QLabel genericStartLabel;
            QLabel genericEndLabel;
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
            bool computation();

            QString getTitle() const;
            void setTitle(const QString &value);

            QSettings *getSettings() const;
            void setSettings(QSettings *value);

            void done(int r);
            void displayPeriod(const QString value);
            void changeDate(const QDate newDate);
            void listElaboration(const QString value);
            void listSecondElab(const QString value);
            void activeSecondParameter(const QString value);
            void readParameter(int state);


    };


#endif // DIALOGWINDOWS_H
