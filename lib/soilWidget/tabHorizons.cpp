#include "tabHorizons.h"

TabHorizons::TabHorizons()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    tableDb = new QTableWidget();
    tableDb->setColumnCount(10);
    QStringList tableDbHeader;
    tableDbHeader << "Upper depth [cm]" << "Lower depth [cm]" << "Sand [%]" << "Silt  [%]" << "Clay [%]" << "Coarse frag. [%]" << "Org. matter [%]"
                    << "Bulk density [g/cm3]" << "Theta S [-]" << "K Sat [cm/d]";
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


    mainLayout->addWidget(tableDb);
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
        tableDb->setItem(i, 8, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.thetaSat)));
        tableDb->setItem(i, 9, new QTableWidgetItem( QString::number(mySoil.horizon[i].dbData.kSat )));

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

//        if (checkHorizonDBData(mySoil, i))
//        {
//            tableModel->item(i,0)->setBackgroundColor(Qt::red);
//            tableModel->item(i,1)->setBackgroundColor(Qt::red);
//            tableModel->item(i,2)->setBackgroundColor(Qt::red);
//            tableModel->item(i,3)->setBackgroundColor(Qt::red);
//            tableModel->item(i,4)->setBackgroundColor(Qt::red);
//            tableModel->item(i,5)->setBackgroundColor(Qt::red);
//            tableModel->item(i,6)->setBackgroundColor(Qt::red);
//            tableModel->item(i,7)->setBackgroundColor(Qt::red);
//            tableModel->item(i,8)->setBackgroundColor(Qt::red);
//            tableModel->item(i,9)->setBackgroundColor(Qt::red);
//            tableModel->item(i,10)->setBackgroundColor(Qt::red);
//        }

    }

}

bool TabHorizons::checkHorizonDBData(soil::Crit3DSoil mySoil, int horizonNum)
{
    bool error = false;
    if (mySoil.horizon[horizonNum].dbData.upperDepth > mySoil.horizon[horizonNum].dbData.lowerDepth)
    {
        tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
        tableDb->item(horizonNum,1)->setBackgroundColor(Qt::red);
        error = true;
    }
    if (horizonNum > 0 && mySoil.horizon[horizonNum].dbData.upperDepth != mySoil.horizon[horizonNum-1].dbData.lowerDepth)
    {
        tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
        error = true;
    }

    if (mySoil.horizon[horizonNum].dbData.sand + mySoil.horizon[horizonNum].dbData.silt + mySoil.horizon[horizonNum].dbData.clay != 100)
    {
        tableDb->item(horizonNum,2)->setBackgroundColor(Qt::red);
        tableDb->item(horizonNum,3)->setBackgroundColor(Qt::red);
        tableDb->item(horizonNum,4)->setBackgroundColor(Qt::red);
        error = true;
    }
    return error;
}
