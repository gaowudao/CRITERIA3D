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
            QHBoxLayout elaborationLayout;
            QHBoxLayout secondElabLayout;
            QComboBox elaborationList;
            QComboBox secondElabList;

        public:
            ComputationDialog(QWidget *parent = 0);
            bool computation();

            QString getTitle() const;
            void setTitle(const QString &value);

            QSettings *getSettings() const;
            void setSettings(QSettings *value);

            void listElaboration(const QString value);

    };


#endif // DIALOGWINDOWS_H
