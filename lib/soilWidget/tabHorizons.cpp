#include "tabHorizons.h"
#include "commonConstants.h"

TabHorizons::TabHorizons()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QLabel* dbTableLabel = new QLabel("Soil data from DB:");
    QLabel* modelTableLabel = new QLabel("Soil parameters estimated by model:");
    tableDb = new QTableWidget();
    tableDb->setColumnCount(10);
    QStringList tableDbHeader;
    tableDbHeader << "Upper depth [cm]" << "Lower depth [cm]" << "Sand [%]" << "Silt  [%]" << "Clay [%]" << "Coarse frag. [%]" << "Org. matter [%]"
                    << "Bulk density [g/cm3]" << "K Sat [cm/d]" << "Theta S [-]";
    tableDb->setHorizontalHeaderLabels(tableDbHeader);
    tableDb->resizeColumnsToContents();
    tableDb->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableDb->setSelectionMode(QAbstractItemView::SingleSelection);
    tableDb->setShowGrid(true);
    tableDb->setStyleSheet("QTableView {selection-background-color: red;}");


    tableModel = new QTableWidget();
    tableModel->setColumnCount(11);
    QStringList tableModelHeader;
    tableModelHeader << "USDA Texture" << "Coarse frag. [%]" << "Org. matter [%]"
                    << "Bulk density [g/cm3]" << "K Sat [cm/d]" << "Theta R [-]" << "Theta S [-]" << "Air entry [KPa]"
                    << "alpha [KPa^-1]" << "n  [-]" << "m   [-]";
    tableModel->setHorizontalHeaderLabels(tableModelHeader);
    tableModel->resizeColumnsToContents();
    tableModel->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableModel->setSelectionMode(QAbstractItemView::SingleSelection);
    tableModel->setShowGrid(true);
    tableModel->setStyleSheet("QTableView {selection-background-color: red;}");


    mainLayout->addWidget(dbTableLabel);
    mainLayout->addWidget(tableDb);
    mainLayout->addWidget(modelTableLabel);
    mainLayout->addWidget(tableModel);

    setLayout(mainLayout);
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
        tableDb->setItem(i, 9, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.thetaSat)));

        tableModel->setItem(i, 0, new QTableWidgetItem( QString::fromStdString(mySoil.horizon[i].texture.classNameUSDA)));
        tableModel->setItem(i, 1, new QTableWidgetItem( QString::number(mySoil.horizon[i].coarseFragments*100 )));
        tableModel->setItem(i, 2, new QTableWidgetItem( QString::number(mySoil.horizon[i].organicMatter*100 )));
        tableModel->setItem(i, 3, new QTableWidgetItem( QString::number(mySoil.horizon[i].bulkDensity )));
        tableModel->setItem(i, 4, new QTableWidgetItem( QString::number(mySoil.horizon[i].waterConductivity.kSat )));
        tableModel->setItem(i, 5, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.thetaR )));
        tableModel->setItem(i, 6, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.thetaS )));
        tableModel->setItem(i, 7, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.he )));
        tableModel->setItem(i, 8, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.alpha )));
        tableModel->setItem(i, 9, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.n )));
        tableModel->setItem(i, 10, new QTableWidgetItem( QString::number(mySoil.horizon[i].vanGenuchten.m )));

        checkHorizonDBData(mySoil, i);
        checkComputedValues(mySoil, i);
        checkMissingItem(i);
    }


}

void TabHorizons::checkHorizonDBData(soil::Crit3DSoil mySoil, int horizonNum)
{
    if (mySoil.horizon[horizonNum].dbData.upperDepth > mySoil.horizon[horizonNum].dbData.lowerDepth)
    {
        tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
        tableDb->item(horizonNum,1)->setBackgroundColor(Qt::red);
    }
    if (horizonNum > 0 && mySoil.horizon[horizonNum].dbData.upperDepth != mySoil.horizon[horizonNum-1].dbData.lowerDepth)
    {
        tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
    }

    if (mySoil.horizon[horizonNum].dbData.sand + mySoil.horizon[horizonNum].dbData.silt + mySoil.horizon[horizonNum].dbData.clay != 100)
    {
        tableDb->item(horizonNum,2)->setBackgroundColor(Qt::red);
        tableDb->item(horizonNum,3)->setBackgroundColor(Qt::red);
        tableDb->item(horizonNum,4)->setBackgroundColor(Qt::red);

        tableModel->item(horizonNum,0)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,1)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,2)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,3)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,4)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,5)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,6)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,7)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,8)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,9)->setBackgroundColor(Qt::red);
        tableModel->item(horizonNum,10)->setBackgroundColor(Qt::red);
    }

}

void TabHorizons::checkMissingItem(int horizonNum)
{
    for (int j = 0; j < tableDb->columnCount(); j++)
    {
        if (tableDb->item(horizonNum,j)->text().toInt() == NODATA)
        {
            tableDb->item(horizonNum,j)->setBackgroundColor(Qt::yellow);
        }
    }

    for (int j = 0; j < tableModel->columnCount(); j++)
    {
        if (tableModel->item(horizonNum,j)->text().toInt() == NODATA)
        {
            tableModel->item(horizonNum,j)->setBackgroundColor(Qt::red);
        }
    }

}

void TabHorizons::checkComputedValues(soil::Crit3DSoil mySoil, int horizonNum)
{
    if (mySoil.horizon[horizonNum].dbData.coarseFragments != mySoil.horizon[horizonNum].coarseFragments*100)
    {
        tableModel->item(horizonNum,1)->setBackgroundColor(Qt::yellow);
    }
    if (mySoil.horizon[horizonNum].dbData.organicMatter != mySoil.horizon[horizonNum].organicMatter*100)
    {
        tableModel->item(horizonNum,2)->setBackgroundColor(Qt::yellow);
    }
    if (mySoil.horizon[horizonNum].dbData.bulkDensity != mySoil.horizon[horizonNum].bulkDensity)
    {
        tableModel->item(horizonNum,3)->setBackgroundColor(Qt::yellow);
    }
    if (mySoil.horizon[horizonNum].dbData.thetaSat != mySoil.horizon[horizonNum].vanGenuchten.thetaS)
    {
        tableModel->item(horizonNum,6)->setBackgroundColor(Qt::yellow);
    }
    if (mySoil.horizon[horizonNum].dbData.kSat != mySoil.horizon[horizonNum].waterConductivity.kSat)
    {
        tableModel->item(horizonNum,4)->setBackgroundColor(Qt::yellow);
    }
}
