#include "tabHorizons.h"

TabHorizons::TabHorizons()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    tableDb = new QTableWidget();
    tableDb->setColumnCount(9);
    QStringList tableDbHeader;
    tableDbHeader << "Upper depth [cm]" << "Lower depth [cm]" << "Sand [%]" << "Silt [%]" << "Clay [%]" << "Coarse fragments [%]" << "Organic matter [%]"
                << "Bulk density [g/cm3]" << "K Sat [cm/d]";
    tableDb->setHorizontalHeaderLabels(tableDbHeader);
    tableDb->resizeColumnsToContents();
    tableDb->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableDb->setSelectionMode(QAbstractItemView::SingleSelection);
    tableDb->setShowGrid(true);
    tableDb->setStyleSheet("QTableView {selection-background-color: red;}");


    tableModel = new QTableWidget();
    tableModel->setColumnCount(11);
    QStringList tableModelHeader;
    tableModelHeader << "Upper depth [cm]" << "Lower depth [cm]" << "Coarse fragments [%]" << "Organic matter [%]"
                << "Bulk density [g/cm3]" << "K Sat [cm/d]" << "Theta R [m3/m3]" << "Theta S [m3/m3]"
                << "alfa [KPa^-1]" << "n [-]" << "m [-]";
    tableModel->setHorizontalHeaderLabels(tableModelHeader);
    tableModel->resizeColumnsToContents();
    tableModel->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableModel->setSelectionMode(QAbstractItemView::SingleSelection);
    tableModel->setShowGrid(true);
    tableModel->setStyleSheet("QTableView {selection-background-color: red;}");


    mainLayout->addWidget(tableDb);
    mainLayout->addWidget(tableModel);
    mainLayout->addWidget(&soilTextEdit);

    setLayout(mainLayout);
}

// example
void TabHorizons::fillTextEdit(QString soilCode, soil::Crit3DSoil mySoil)
{

    soilTextEdit.clear();
    // show data (example)
    soilTextEdit.append(soilCode);
    soilTextEdit.append("Horizon nr., sand (%),   silt (%),   clay (%)");
    for (int i = 0; i < mySoil.nrHorizons; i++)
    {
        QString s;
        s = QString::number(mySoil.horizon[i].dbData.horizonNr)
                + "\t" + QString::number(mySoil.horizon[i].dbData.sand)
                + "\t" + QString::number(mySoil.horizon[i].dbData.silt)
                + "\t" + QString::number(mySoil.horizon[i].dbData.clay);
        soilTextEdit.append(s);
    }

}
