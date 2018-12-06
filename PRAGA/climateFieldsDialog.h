#ifndef CLIMATEFIELDSDIALOG_H
#define CLIMATEFIELDSDIALOG_H

#include <QtWidgets>
#include "meteo.h"

class ClimateFieldsDialog : public QDialog
{
    Q_OBJECT

    private:

        QWidget elabW;
        QWidget indexW;

        QStringList climateDbVarList;
        QStringList climateDbElab;

        QListWidget listVariable;
        QListWidget listElab;
        QListWidget listIndex;

        QHBoxLayout mainLayout;
        QVBoxLayout variableLayout;
        QVBoxLayout elabLayout;
        QVBoxLayout indexLayout;

        QString climaSelected;
        meteoVariable var;
        QString indexSelected;

    public:
        ClimateFieldsDialog(QStringList climateDbElab, QStringList climateDbVarList);
        void variableClicked(QListWidgetItem *item);
        void elabClicked(QListWidgetItem* item);
        void indexClicked(QListWidgetItem* item);
        QString getSelected() const;
        meteoVariable getVar() const;
};

#endif // CLIMATEFIELDSDIALOG_H
