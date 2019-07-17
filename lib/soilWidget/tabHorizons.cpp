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

void TabHorizons::insertSoilHorizons(soil::Crit3DSoil mySoil)
{
    int row = mySoil.nrHorizons;
    tableDb->setRowCount(row);
    tableModel->setRowCount(row);
    for (int i = 0; i < row; i++)
    {
        tableDb->setItem(i, 0, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.upperDepth )));
        tableDb->setItem(i, 1, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.lowerDepth )));
        tableDb->setItem(i, 2, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.sand )));
        tableDb->setItem(i, 3, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.silt )));
        tableDb->setItem(i, 4, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.clay )));
        tableDb->setItem(i, 5, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.coarseFragments )));
        tableDb->setItem(i, 6, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.organicMatter )));
        tableDb->setItem(i, 7, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.bulkDensity )));
        tableDb->setItem(i, 8, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.kSat )));

        tableModel->setItem(i, 0, new QTableWidgetItem( QString::number(mySoil.horizon[i].upperDepth )));
        tableModel->setItem(i, 1, new QTableWidgetItem( QString::number(mySoil.horizon[i].lowerDepth )));
        tableModel->setItem(i, 2, new QTableWidgetItem( QString::number(mySoil.horizon[i].coarseFragments )));
        tableModel->setItem(i, 3, new QTableWidgetItem( QString::number(mySoil.horizon[i].organicMatter )));
        tableModel->setItem(i, 4, new QTableWidgetItem( QString::number(mySoil.horizon[i].bulkDensity )));
        tableModel->setItem(i, 5, new QTableWidgetItem( QString::number(mySoil.horizon[i].waterConductivity.kSat )));
        tableModel->setItem(i, 6, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.thetaR )));
        tableModel->setItem(i, 7, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.thetaS )));
        tableModel->setItem(i, 8, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.alpha )));
        tableModel->setItem(i, 9, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.n )));
        tableModel->setItem(i, 10, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.m )));
    }

}
