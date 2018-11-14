#ifndef CLIMATEFIELDSDIALOG_H
#define CLIMATEFIELDSDIALOG_H

#include <QtWidgets>
#include "meteo.h"

class ClimateFieldsDialog : public QDialog
{
    Q_OBJECT

    private:
        QStringList climateDbElab;
        QStringList climateDbVarList;

        QListWidget listVariable;
        QListWidget listElab;

        QHBoxLayout mainLayout;
        QVBoxLayout variableLayout;
        QVBoxLayout elabLayout;

        QString selected;
        meteoVariable var;

    public:
        ClimateFieldsDialog(QStringList climateDbElab, QStringList climateDbVarList);
        void itemClicked(QListWidgetItem *item);
        void endSelection(QListWidgetItem* item);
        QString getSelected() const;
        meteoVariable getVar() const;
};

#endif // CLIMATEFIELDSDIALOG_H
