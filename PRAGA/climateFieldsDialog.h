#ifndef CLIMATEFIELDSDIALOG_H
#define CLIMATEFIELDSDIALOG_H

#include <QtWidgets>

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

    public:
        ClimateFieldsDialog(QStringList climateDbElab, QStringList climateDbVarList);
        void itemClicked(QListWidgetItem *item);
        void endSelection(QListWidgetItem* item);
        QString getSelected() const;
};

#endif // CLIMATEFIELDSDIALOG_H
