#include "tabHorizons.h"
#include "commonConstants.h"
#include "soil.h"


TabHorizons::TabHorizons()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QLabel* dbTableLabel = new QLabel("Soil parameters from DB:");
    dbTableLabel->setStyleSheet("font: 11pt;");
    QLabel* modelTableLabel = new QLabel("Soil parameters estimated by model:");
    modelTableLabel->setStyleSheet("font: 11pt;");
    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableDb = new Crit3DSoilTable(dbTable);
    tableModel = new Crit3DSoilTable(modelTable);
    QHBoxLayout *addDeleteRowLayout = new QHBoxLayout;
    QLabel* addDeleteLabel = new QLabel("Modify horizons:");
    addRow = new QPushButton("+");
    addRow->setFixedWidth(40);
    deleteRow = new QPushButton("-");
    deleteRow->setFixedWidth(40);
    addRow->setEnabled(false);
    deleteRow->setEnabled(false);
    addDeleteRowLayout->addStretch(40);
    addDeleteRowLayout->addWidget(addDeleteLabel);
    addDeleteRowLayout->addWidget(addRow);
    addDeleteRowLayout->addWidget(deleteRow);

    // tableModel with a Width perfectly wrapping all columns, tableDb copy the same size, add margin because now the table is empty
    tableDb->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableModel->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    int margin = 25;
    tableModel->setFixedWidth(tableModel->horizontalHeader()->length() + tableModel->verticalHeader()->width() + margin);
    tableDb->setFixedWidth(tableModel->width());

    connect(tableDb->verticalHeader(), &QHeaderView::sectionClicked, [=](int index){ this->tableDbVerticalHeaderClick(index); });
    connect(tableModel->verticalHeader(), &QHeaderView::sectionClicked, [=](int index){ this->tableDbVerticalHeaderClick(index); });
    connect(addRow, &QPushButton::clicked, [=](){ this->addRowClicked(); });
    connect(deleteRow, &QPushButton::clicked, [=](){ this->removeRowClicked(); });

    tableLayout->addWidget(dbTableLabel);
    tableLayout->addWidget(tableDb);
    tableLayout->addLayout(addDeleteRowLayout);
    tableLayout->addWidget(modelTableLabel);
    tableLayout->addWidget(tableModel);

    mainLayout->addWidget(barHorizons.groupBox);
    mainLayout->addLayout(tableLayout);

    setLayout(mainLayout);
    insertSoilElement = false;
    soilCodeChanged = false;
}

void TabHorizons::insertSoilHorizons(soil::Crit3DSoil *soil, soil::Crit3DTextureClass* textureClassList,
                                     soil::Crit3DFittingOptions* fittingOptions)
{
    if (soil == nullptr)
    {
        return;
    }

    resetAll();

    int totHeight = int(this->height() * 0.85);
    barHorizons.draw(totHeight, soil);

    insertSoilElement = true;
    //disable events otherwise setBackgroundColor call again cellChanged event
    tableDb->blockSignals(true);
    mySoil = soil;
    myTextureClassList = textureClassList;
    myFittingOptions = fittingOptions;

    int row = mySoil->nrHorizons;
    tableDb->setRowCount(row);
    tableModel->setRowCount(row);

    // fill Tables
    for (int i = 0; i < row; i++)
    {
        tableDb->setItem(i, 0, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.upperDepth, 'f', 0)));
        tableDb->setItem(i, 1, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.lowerDepth, 'f', 0)));
        tableDb->setItem(i, 2, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.sand, 'f', 1 )));
        tableDb->setItem(i, 3, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.silt, 'f', 1 )));
        tableDb->setItem(i, 4, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.clay , 'f', 1)));
        tableDb->setItem(i, 5, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.coarseFragments, 'f', 1 )));
        tableDb->setItem(i, 6, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.organicMatter, 'f', 1 )));
        tableDb->setItem(i, 7, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.bulkDensity, 'f', 3 )));
        tableDb->setItem(i, 8, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.kSat, 'f', 3 )));
        tableDb->setItem(i, 9, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.thetaSat, 'f', 3)));

        tableModel->setItem(i, 0, new QTableWidgetItem( QString::fromStdString(mySoil->horizon[i].texture.classNameUSDA)));
        if (mySoil->horizon[i].coarseFragments != NODATA)
        {
            tableModel->setItem(i, 1, new QTableWidgetItem( QString::number(mySoil->horizon[i].coarseFragments*100, 'f', 1 )));
        }
        else
        {
            tableModel->setItem(i, 1, new QTableWidgetItem( QString::number(mySoil->horizon[i].coarseFragments, 'f', 1 )));
        }

        if (mySoil->horizon[i].organicMatter != NODATA)
        {
            tableModel->setItem(i, 2, new QTableWidgetItem( QString::number(mySoil->horizon[i].organicMatter*100, 'f', 1 )));
        }
        else
        {
            tableModel->setItem(i, 2, new QTableWidgetItem( QString::number(mySoil->horizon[i].organicMatter, 'f', 1 )));
        }

        tableModel->setItem(i, 3, new QTableWidgetItem( QString::number(mySoil->horizon[i].bulkDensity, 'f', 3 )));
        tableModel->setItem(i, 4, new QTableWidgetItem( QString::number(mySoil->horizon[i].waterConductivity.kSat, 'f', 3 )));
        tableModel->setItem(i, 5, new QTableWidgetItem( QString::number(mySoil->horizon[i].vanGenuchten.thetaS, 'f', 3 )));
        tableModel->setItem(i, 6, new QTableWidgetItem( QString::number(mySoil->horizon[i].vanGenuchten.thetaR, 'f', 3 )));
        tableModel->setItem(i, 7, new QTableWidgetItem( QString::number(mySoil->horizon[i].vanGenuchten.he, 'f', 3 )));
        tableModel->setItem(i, 8, new QTableWidgetItem( QString::number(mySoil->horizon[i].vanGenuchten.alpha, 'f', 3 )));
        tableModel->setItem(i, 9, new QTableWidgetItem( QString::number(mySoil->horizon[i].vanGenuchten.n, 'f', 3 )));
        tableModel->setItem(i, 10, new QTableWidgetItem( QString::number(mySoil->horizon[i].vanGenuchten.m, 'f', 3 )));   
    }

    // check all Depths
    checkDepths();
    // check other values
    for (int i = 0; i < row; i++)
    {
        checkMissingItem(i);
        if (checkHorizonData(i))
        {
            checkComputedValues(i);
        }
    }

    tableDb->blockSignals(false);
    addRow->setEnabled(true);
    deleteRow->setEnabled(false);

    connect(tableDb, &QTableWidget::cellDoubleClicked, [=](int row, int column){ this->editItem(row, column); });
    connect(tableDb, &QTableWidget::cellChanged, [=](int row, int column){ this->cellChanged(row, column); });
    connect(tableDb, &QTableWidget::cellClicked, [=](int row, int column){ this->cellClickedDb(row, column); });
    connect(tableModel, &QTableWidget::cellClicked, [=](int row, int column){ this->cellClickedModel(row, column); });
    for (int i=0; i<barHorizons.list.size(); i++)
    {
        connect(barHorizons.list[i], SIGNAL(clicked(int)), this, SLOT(widgetClicked(int)));
    }
    cellClickedDb(0,0);


}

void TabHorizons::checkDepths()
{
    // reset background color
    for (int horizonNum = 0; horizonNum<tableDb->rowCount(); horizonNum++)
    {
        tableDb->item(horizonNum,0)->setBackgroundColor(Qt::white);
        tableDb->item(horizonNum,1)->setBackgroundColor(Qt::white);
    }
    for (int horizonNum = 0; horizonNum<tableDb->rowCount(); horizonNum++)
    {
        //except first row
        if ( horizonNum > 0)
        {
            if (mySoil->horizon[horizonNum].dbData.upperDepth != mySoil->horizon[horizonNum-1].dbData.lowerDepth)
            {
                tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
                tableDb->item(horizonNum-1,1)->setBackgroundColor(Qt::red);
            }
        }

        // except last row
        if (horizonNum < tableDb->rowCount()-1)
        {
            if (mySoil->horizon[horizonNum].dbData.lowerDepth != mySoil->horizon[horizonNum+1].dbData.upperDepth)
            {
                tableDb->item(horizonNum,1)->setBackgroundColor(Qt::red);
                tableDb->item(horizonNum+1,0)->setBackgroundColor(Qt::red);
            }
        }

        if (mySoil->horizon[horizonNum].dbData.upperDepth == NODATA || mySoil->horizon[horizonNum].dbData.lowerDepth == NODATA)
        {
            tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
            tableDb->item(horizonNum,1)->setBackgroundColor(Qt::red);
        }
        else if (mySoil->horizon[horizonNum].dbData.upperDepth > mySoil->horizon[horizonNum].dbData.lowerDepth)
        {
            tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
            tableDb->item(horizonNum,1)->setBackgroundColor(Qt::red);
        }
        else
        {
            if (mySoil->horizon[horizonNum].dbData.upperDepth < 0)
            {
                tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
            }
            if (mySoil->horizon[horizonNum].dbData.lowerDepth < 0)
            {
                tableDb->item(horizonNum,1)->setBackgroundColor(Qt::red);
            }
        }
    }
}


bool TabHorizons::checkHorizonData(int horizonNum)
{
    bool goOn = true;
    soil::Crit3DHorizonDbData* dbData = &(mySoil->horizon[horizonNum].dbData);

    if (soil::getUSDATextureClass(dbData->sand, dbData->silt, dbData->clay) == NODATA)
    {
        tableDb->item(horizonNum,2)->setBackgroundColor(Qt::red);
        tableDb->item(horizonNum,3)->setBackgroundColor(Qt::red);
        tableDb->item(horizonNum,4)->setBackgroundColor(Qt::red);

        setInvalidTableModelRow(horizonNum);
        goOn = false;
    }

    if (dbData->coarseFragments != NODATA && (dbData->coarseFragments < 0 || dbData->coarseFragments >= 100))
    {
        tableDb->item(horizonNum,5)->setBackgroundColor(Qt::red);
    }

    if (dbData->organicMatter != NODATA && (dbData->organicMatter < 0 || dbData->organicMatter > 100))
    {
        tableDb->item(horizonNum,6)->setBackgroundColor(Qt::red);
    }

    if (dbData->bulkDensity != NODATA && (dbData->bulkDensity <= 0 || dbData->bulkDensity > QUARTZ_DENSITY))
    {
        tableDb->item(horizonNum,7)->setBackgroundColor(Qt::red);
    }

    if (dbData->kSat != NODATA && dbData->kSat <= 0)
    {
        tableDb->item(horizonNum,8)->setBackgroundColor(Qt::red);
    }

    if (dbData->thetaSat != NODATA && (dbData->thetaSat <= 0 || dbData->thetaSat >= 1))
    {
        tableDb->item(horizonNum,9)->setBackgroundColor(Qt::red);
    }

    return goOn;
}


void TabHorizons::setInvalidTableModelRow(int horizonNum)
{
    tableModel->item(horizonNum,0)->setText("UNDEFINED");
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

void TabHorizons::checkMissingItem(int horizonNum)
{
    QString NODATAString = "-9999";
    // except for lower/upper depths
    for (int j = 2; j < tableDb->columnCount(); j++)
    {
        if (tableDb->item(horizonNum,j)->text().contains(NODATAString) || tableDb->item(horizonNum,j)->text().isEmpty())
        {
            tableDb->item(horizonNum,j)->setBackgroundColor(Qt::yellow);
            tableDb->item(horizonNum,j)->setText("");
        }
    }

    for (int j = 0; j < tableModel->columnCount(); j++)
    {
        if (tableModel->item(horizonNum,j)->text().contains(NODATAString) || tableModel->item(horizonNum,j)->text().isEmpty())
        {
            tableModel->item(horizonNum,j)->setBackgroundColor(Qt::red);
            tableModel->item(horizonNum,j)->setText("");
        }
    }
}


void TabHorizons::checkComputedValues(int horizonNum)
{
    soil::Crit3DHorizon * horizon = &(mySoil->horizon[unsigned(horizonNum)]);

    if (abs(horizon->dbData.coarseFragments - horizon->coarseFragments*100) > EPSILON)
    {
        tableModel->item(horizonNum,1)->setBackgroundColor(Qt::yellow);
    }

    if (abs(horizon->dbData.organicMatter - horizon->organicMatter*100) > EPSILON)
    {
        tableModel->item(horizonNum,2)->setBackgroundColor(Qt::yellow);
    }

    if (abs(horizon->dbData.bulkDensity - horizon->bulkDensity) > EPSILON)
    {
        tableModel->item(horizonNum,3)->setBackgroundColor(Qt::yellow);
    }

    if (abs(horizon->dbData.kSat - horizon->waterConductivity.kSat) > EPSILON)
    {
        tableModel->item(horizonNum,4)->setBackgroundColor(Qt::yellow);
    }

    if (abs(horizon->dbData.thetaSat - horizon->vanGenuchten.thetaS) > EPSILON)
    {
        tableModel->item(horizonNum,5)->setBackgroundColor(Qt::yellow);
    }
}


void TabHorizons::clearSelections()
{
    tableDb->clearSelection();
    tableModel->clearSelection();
    deleteRow->setEnabled(false);
    emit horizonSelected(-1);
}

void TabHorizons::editItem(int row, int column)
{
    tableDb->itemAt(row,column)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable);
}

void TabHorizons::cellClickedDb(int row, int column)
{
    clearSelections();
    tableDb->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableModel->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableDb->setItemSelected(tableDb->item(row,column), true);

    switch (column) {
        case 5:
        {
            tableModel->setItemSelected(tableModel->item(row,1), true);
            break;
        }
        case 6:
        {
            tableModel->setItemSelected(tableModel->item(row,2), true);
            break;
        }
        case 7:
        {
            tableModel->setItemSelected(tableModel->item(row,3), true);
            break;
        }
        case 8:
        {
            tableModel->setItemSelected(tableModel->item(row,4), true);
            break;
        }
        case 9:
        {
            tableModel->setItemSelected(tableModel->item(row,5), true);
            break;
        }
    }

    barHorizons.selectItem(row);

    deleteRow->setEnabled(false);
    emit horizonSelected(row);
}

void TabHorizons::cellClickedModel(int row, int column)
{

    clearSelections();
    tableDb->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableModel->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableModel->setItemSelected(tableModel->item(row,column), true);

    switch (column) {
        case 1:
        {
            tableDb->setItemSelected(tableDb->item(row,5), true);
            break;
        }
        case 2:
        {
            tableDb->setItemSelected(tableDb->item(row,6), true);
            break;
        }
        case 3:
        {
            tableDb->setItemSelected(tableDb->item(row,7), true);
            break;
        }
        case 4:
        {
            tableDb->setItemSelected(tableDb->item(row,8), true);
            break;
        }
        case 5:
        {
            tableDb->setItemSelected(tableDb->item(row,9), true);
            break;
        }
    }

    barHorizons.selectItem(row);

    deleteRow->setEnabled(false);
    emit horizonSelected(row);
}

void TabHorizons::tableDbVerticalHeaderClick(int index)
{
    tableDb->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableModel->setSelectionBehavior(QAbstractItemView::SelectRows);

    tableDb->selectRow(index);
    tableDb->horizontalHeader()->setHighlightSections(false);
    tableModel->selectRow(index);
    tableModel->horizontalHeader()->setHighlightSections(false);
    deleteRow->setEnabled(true);

    barHorizons.selectItem(index);

    emit horizonSelected(index);

}

void TabHorizons::cellChanged(int row, int column)
{

    if (tableDb->itemAt(row,column) == nullptr || mySoil->nrHorizons < row)
    {
        qDebug() << "mySoil->horizon->dbData.horizonNr < row ";
        return;
    }

    //disable events otherwise setBackgroundColor call again cellChanged event
    tableDb->blockSignals(true);
    tableModel->selectRow(row);
    QString data = tableDb->item(row, column)->text();
    data.replace(",", ".");

    // set new value
    switch (column) {
        case 0:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.upperDepth = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.upperDepth = data.toDouble();
                tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 0));
            }
            break;
        }
        case 1:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.lowerDepth = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.lowerDepth = data.toDouble();
                tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 0));
            }
            break;
        }
        case 2:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.sand = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.sand = data.toFloat();
                tableDb->item(row, column)->setText(QString::number(data.toFloat(), 'f', 1));
            }
            break;
        }
        case 3:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.silt = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.silt = data.toFloat();
                tableDb->item(row, column)->setText(QString::number(data.toFloat(), 'f', 1));
            }
            break;
        }
        case 4:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.clay = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.clay = data.toFloat();
                tableDb->item(row, column)->setText(QString::number(data.toFloat(), 'f', 1));
            }
            break;
        }
        case 5:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.coarseFragments = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.coarseFragments = data.toDouble();
                tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 1));
            }
            break;
        }
        case 6:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.organicMatter = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.organicMatter = data.toDouble();
                tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 1));
            }
            break;
        }
        case 7:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.bulkDensity = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.bulkDensity = data.toDouble();
                tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 3));
            }
            break;
        }
        case 8:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.kSat = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.kSat = data.toDouble();
                tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 3));
            }
            break;
        }
        case 9:
        {
            if (data == NODATA || data.isEmpty())
            {
                mySoil->horizon[row].dbData.thetaSat = NODATA;
                tableDb->item(row, column)->setText("");
            }
            else
            {
                mySoil->horizon[row].dbData.thetaSat = data.toDouble();
                tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 3));
            }
            break;
        }
    }

    std::string errorString;
    soil::setHorizon(&(mySoil->horizon[row]), myTextureClassList, myFittingOptions, &errorString);

    // update tableModel values
    tableModel->item(row,0)->setText(QString::fromStdString(mySoil->horizon[row].texture.classNameUSDA));

    if (mySoil->horizon[row].coarseFragments != NODATA)
    {
        tableModel->item(row,1)->setText(QString::number(mySoil->horizon[row].coarseFragments*100, 'f', 1 ));
    }
    else
    {
        tableModel->item(row,1)->setText("");
    }

    if (mySoil->horizon[row].organicMatter != NODATA)
    {
        tableModel->item(row,2)->setText(QString::number(mySoil->horizon[row].organicMatter*100, 'f', 1 ));
    }
    else
    {
        tableModel->item(row,2)->setText("");
    }

    tableModel->item(row,3)->setText(QString::number(mySoil->horizon[row].bulkDensity, 'f', 3));
    tableModel->item(row,4)->setText(QString::number(mySoil->horizon[row].waterConductivity.kSat, 'f', 3));
    tableModel->item(row,5)->setText(QString::number(mySoil->horizon[row].vanGenuchten.thetaS, 'f', 3));
    tableModel->item(row,6)->setText(QString::number(mySoil->horizon[row].vanGenuchten.thetaR, 'f', 3));
    tableModel->item(row,7)->setText(QString::number(mySoil->horizon[row].vanGenuchten.he, 'f', 3));
    tableModel->item(row,8)->setText(QString::number(mySoil->horizon[row].vanGenuchten.alpha, 'f', 3));
    tableModel->item(row,9)->setText(QString::number(mySoil->horizon[row].vanGenuchten.n, 'f', 3));
    tableModel->item(row,10)->setText(QString::number(mySoil->horizon[row].vanGenuchten.m, 'f', 3));

    // reset background color for the row changed
    for (int j = 0; j < tableDb->columnCount(); j++)
    {
        tableDb->item(row,j)->setBackgroundColor(Qt::white);
    }

    for (int j = 0; j < tableModel->columnCount(); j++)
    {
        tableModel->item(row,j)->setBackgroundColor(Qt::white);
    }

    // check all Depths
    checkDepths();
    // check new values and assign background color
    checkMissingItem(row);
    if (checkHorizonData(row))
    {
        checkComputedValues(row);
    }

    clearSelections();
    tableDb->blockSignals(false);
    soilCodeChanged = true;
}

void TabHorizons::addRowClicked()
{
    tableDb->blockSignals(true);
    int numRow;

    if (tableDb->rowCount() != 0)
    {
        if (tableDb->selectedItems().isEmpty())
        {
            QMessageBox::critical(nullptr, "Warning", "Select the row of the horizon before the one you want to add");
            return;
        }
        else
        {
            if (tableDb->selectedItems().size() != tableDb->columnCount())
            {
                QMessageBox::critical(nullptr, "Warning", "Select the row of the horizon before the one you want to add");
                return;
            }
            numRow = tableDb->selectedItems().at(0)->row()+1;
        }
    }
    else
    {
        numRow = 0;
    }

    tableDb->insertRow(numRow);
    tableModel->insertRow(numRow);

    for (int j=0; j<tableDb->columnCount(); j++)
    {
        tableDb->setItem(numRow, j, new QTableWidgetItem());
    }
    for (int j=0; j<tableModel->columnCount(); j++)
    {
        tableModel->setItem(numRow, j, new QTableWidgetItem());
    }
    deleteRow->setEnabled(true);

    setInvalidTableModelRow(numRow);

    soil::Crit3DHorizon* newHor = new soil::Crit3DHorizon();
    // set newHor dbData
    newHor->dbData.horizonNr = numRow;
    QString lowerDepth;
    if (numRow != 0)
    {
        lowerDepth = tableDb->item(numRow-1, 1)->text();
        tableDb->item(numRow, 0)->setText(lowerDepth);
        newHor->dbData.upperDepth = lowerDepth.toDouble();
    }
    else
    {
        newHor->dbData.upperDepth = 0;
        tableDb->item(numRow, 0)->setText("0");
    }
    newHor->dbData.lowerDepth = NODATA;
    newHor->dbData.sand = NODATA;
    newHor->dbData.silt = NODATA;
    newHor->dbData.clay = NODATA;
    newHor->dbData.coarseFragments = NODATA;
    newHor->dbData.organicMatter = NODATA;
    newHor->dbData.bulkDensity = NODATA;
    newHor->dbData.thetaSat = NODATA;
    newHor->dbData.kSat = NODATA;

    mySoil->addHorizon(numRow,newHor);
    // check all Depths
    checkDepths();
    // check new values and assign background color
    checkMissingItem(numRow);
    if (checkHorizonData(numRow))
    {
        checkComputedValues(numRow);
    }
    tableDb->blockSignals(false);
    soilCodeChanged = true;

}

void TabHorizons::removeRowClicked()
{
    tableDb->blockSignals(true);
    int row;
    // check if a row is selected
    if (tableDb->selectedItems().isEmpty())
    {
        QMessageBox::critical(nullptr, "Error!", "Select a horizon");
        return;
    }
    if (tableDb->selectedItems().size() == tableDb->columnCount())
    {
        row = tableDb->selectedItems().at(0)->row();
    }
    else
    {
        QMessageBox::critical(nullptr, "Error!", "Select a horizon");
        return;
    }
    tableDb->removeRow(row);
    tableModel->removeRow(row);
    mySoil->deleteHorizon(row);
    checkDepths();
    tableDb->blockSignals(false);
    soilCodeChanged = true;
}

bool TabHorizons::getSoilCodeChanged()
{
    return soilCodeChanged;
}

void TabHorizons::resetSoilCodeChanged()
{
    soilCodeChanged = false;
}

void TabHorizons::resetAll()
{
    // delete all Widgets
    barHorizons.clear();
    tableDb->setRowCount(0);
    tableModel->setRowCount(0);
    insertSoilElement = false;
    clearSelections();
    resetSoilCodeChanged();
}

bool TabHorizons::getInsertSoilElement() const
{
    return insertSoilElement;
}

void TabHorizons::setInsertSoilElement(bool value)
{
    insertSoilElement = value;
}

void TabHorizons::widgetClicked(int index)
{
    // check selection state
    if (barHorizons.list[index]->getSelected())
    {
        // clear previous selection
        for(int i = 0; i < barHorizons.list.size(); i++)
        {
            if (i != index)
            {
                barHorizons.list[i]->restoreFrame();
                barHorizons.list[i]->setSelected(false);
            }
        }
        tableDbVerticalHeaderClick(index);
    }
    else
    {
        clearSelections();
    }

}
