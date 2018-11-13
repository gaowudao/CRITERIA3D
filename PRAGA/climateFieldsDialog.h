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

    public:
        ClimateFieldsDialog(QStringList climateDbElab, QStringList climateDbVarList);
        void itemClicked(QListWidgetItem *item);
};

#endif // CLIMATEFIELDSDIALOG_H
