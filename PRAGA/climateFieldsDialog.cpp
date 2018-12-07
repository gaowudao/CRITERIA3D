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

QString ClimateFieldsDialog::getIndexSelected() const
{
    return indexSelected;
}

bool ClimateFieldsDialog::getIsShowClicked() const
{
    return isShowClicked;
}

ClimateFieldsDialog::ClimateFieldsDialog(QStringList climateDbElab, QStringList climateDbVarList)
: climateDbElab(climateDbElab), climateDbVarList(climateDbVarList)
{
    setWindowTitle("Climate Variables");

    showButton.setText("Show");
    showButton.setEnabled(false);

    deleteButton.setText("Delete");
    deleteButton.setEnabled(false);

    buttonLayout.addWidget(&showButton);
    buttonLayout.addWidget(&deleteButton);

    listVariable.addItems(climateDbVarList);
    variableLayout.addWidget(&listVariable);
    indexLayout.addWidget(&listIndex);

    connect(&listVariable, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->variableClicked(item); });

    connect(&showButton, &QPushButton::clicked, [=](){ showClicked(); });
    connect(&deleteButton, &QPushButton::clicked, [=](){ deleteClicked(); });
    //connect(&buttonBox, &QDialogButtonBox::accepted, [=](){ this->done(true); });
    //connect(&buttonBox, &QDialogButtonBox::rejected, [=](){ this->done(false); });

    mainLayout.addLayout(&variableLayout);

    elabW.setLayout(&elabLayout);
    mainLayout.addWidget(&elabW);
    indexW.setLayout(&indexLayout);
    mainLayout.addWidget(&indexW);
    mainLayout.addLayout(&buttonLayout);

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
    showButton.setEnabled(false);
    deleteButton.setEnabled(false);

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
    showButton.setEnabled(false);
    deleteButton.setEnabled(false);
    QStringList listIndexSelected;

    int n = getNumberClimateIndexFromElab(climaSelected);
    if (n == 1)
    {
        indexSelected = "1";
        indexW.setVisible(false);
        showButton.setEnabled(true);
        deleteButton.setEnabled(true);
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
    showButton.setEnabled(true);
    deleteButton.setEnabled(true);
}

void ClimateFieldsDialog::showClicked()
{
    isShowClicked = true;
    QDialog::done(QDialog::Accepted);
}

void ClimateFieldsDialog::deleteClicked()
{
    isShowClicked = false;
    QDialog::done(QDialog::Accepted);
}
