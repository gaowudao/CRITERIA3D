#ifndef SERIESONZONESDIALOG_H
#define SERIESONZONESDIALOG_H

#include <QString>
#include <QSettings>
#include <QGridLayout>
#include <QComboBox>

#include <QtWidgets>
#include "meteoGrid.h"

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

        meteoVariable variable;
        QDate startDate;
        QDate endDate;
        gridAggregationMethod spatialElaboration;

    public:
        SeriesOnZonesDialog(QSettings *settings);
        void done(bool res);
        bool checkValidData();

        meteoVariable getVariable() const;
        QDate getStartDate() const;
        QDate getEndDate() const;
        gridAggregationMethod getSpatialElaboration() const;
};


#endif // SERIESONZONESDIALOG_H
