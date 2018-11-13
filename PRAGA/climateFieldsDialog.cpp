#include "climateFieldsDialog.h"

ClimateFieldsDialog::ClimateFieldsDialog(QStringList climateDbElab, QStringList climateDbVarList)
: climateDbElab(climateDbElab), climateDbVarList(climateDbVarList)
{
    setWindowTitle("Climate Variables");
    QVBoxLayout mainLayout;

    listVariable.addItems(climateDbVarList);
    mainLayout.addWidget(&listVariable);

    connect(&listVariable, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->itemClicked(item); });
    setLayout(&mainLayout);

    show();
    exec();
}

void ClimateFieldsDialog::itemClicked(QListWidgetItem* item)
{
    qInfo() << "item" << item->text(); // debug

}
