#ifndef SERIESONZONESDIALOG_H
#define SERIESONZONESDIALOG_H

#include <QString>
#include <QSettings>
#include <QGridLayout>
#include <QComboBox>

#include <QtWidgets>

class SeriesOnZonesDialog: public QDialog
{

    Q_OBJECT

    private:
        QSettings* settings;
        QComboBox variableList;
        QLabel genericStartLabel;
        QLabel genericEndLabel;
        QDateEdit genericPeriodStart;
        QDateEdit genericPeriodEnd;
        QComboBox spatialElab;

        QString variable;
        QDate startDate;
        QDate endDate;
        QString spatialElaboration;

    public:
        SeriesOnZonesDialog(QSettings *settings);
        void done(bool res);
        bool checkValidData();

        QString getVariable() const;
        QDate getStartDate() const;
        QDate getEndDate() const;
        QString getSpatialElaboration() const;
};


#endif // SERIESONZONESDIALOG_H
