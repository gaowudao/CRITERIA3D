#include "climateFieldsDialog.h"
#include "climate.h"

QString ClimateFieldsDialog::getSelected() const
{
    return climaSelected;
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
    indexLayout.addWidget(&listIndex);

    connect(&listVariable, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->variableClicked(item); });

    mainLayout.addLayout(&variableLayout);

    elabW.setLayout(&elabLayout);
    mainLayout.addWidget(&elabW);
    indexW.setLayout(&indexLayout);
    mainLayout.addWidget(&indexW);

    elabW.setVisible(false);
    indexW.setVisible(false);


    setLayout(&mainLayout);
    mainLayout.setSizeConstraint(QLayout::SetFixedSize);

    show();
    exec();
}

void ClimateFieldsDialog::variableClicked(QListWidgetItem* item)
{
    elabW.setVisible(true);
    indexW.setVisible(false);

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
    connect(&listElab, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->elabClicked(item); });

}

void ClimateFieldsDialog::elabClicked(QListWidgetItem* item)
{
    climaSelected = item->text();
    listIndex.clear();
    QStringList listIndexSelected;

    int n = getNumberClimateIndexFromElab(climaSelected);
    if (n == 1)
    {
        indexSelected = 1;
        indexW.setVisible(false);
    }
    else
    {
        indexW.setVisible(true);
        for (int i=1; i <= n; i++)
        {
            if (n==4)
            {
                switch(i) {
                    case 1 : listIndexSelected.append("MAM");
                             break;
                    case 2 : listIndexSelected.append("JJA");
                             break;
                    case 3 : listIndexSelected.append("SON");
                             break;
                    case 4 : listIndexSelected.append("DJF");
                             break;
                }

            }
            else
            {
                listIndexSelected.append(QString::number(i));
            }

        }

        listIndex.addItems(listIndexSelected);
        indexLayout.addWidget(&listIndex);
        connect(&listIndex, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->indexClicked(item); });
    }

}


void ClimateFieldsDialog::indexClicked(QListWidgetItem* item)
{
    indexSelected = item->text();
    QDialog::done(QDialog::Accepted);
}
