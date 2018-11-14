#include "climateFieldsDialog.h"

QString ClimateFieldsDialog::getSelected() const
{
    return selected;
}

meteoVariable ClimateFieldsDialog::getVar() const
{
    return var;
}

ClimateFieldsDialog::ClimateFieldsDialog(QStringList climateDbElab, QStringList climateDbVarList)
: climateDbElab(climateDbElab), climateDbVarList(climateDbVarList)
{
    setWindowTitle("Climate Variables");

    listVariable.addItems(climateDbVarList);
    variableLayout.addWidget(&listVariable);

    connect(&listVariable, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->itemClicked(item); });

    mainLayout.addLayout(&variableLayout);
    mainLayout.addLayout(&elabLayout);
    setLayout(&mainLayout);
    mainLayout.setSizeConstraint(QLayout::SetFixedSize);

    show();
    exec();
}

void ClimateFieldsDialog::itemClicked(QListWidgetItem* item)
{

    listElab.clear();
    QStringList elabVarSelected;

    std::string variable = item->text().toStdString();
    var = getKeyMeteoVarMeteoMap(MapDailyMeteoVarToString, variable);

    for (int i=0; i < climateDbElab.size(); i++)
    {
        QString elab = climateDbElab.at(i);
        QStringList words = elab.split('_');
        QString var = words[1];
        if (var == item->text())
        {
            elabVarSelected.append(elab);
        }
    }

    listElab.addItems(elabVarSelected);
    elabLayout.addWidget(&listElab);
    connect(&listElab, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->endSelection(item); });

}

void ClimateFieldsDialog::endSelection(QListWidgetItem* item)
{
    selected = item->text();
    QDialog::done(QDialog::Accepted);
}
