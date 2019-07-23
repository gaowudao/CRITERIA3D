#include "tabHorizons.h"
#include "commonConstants.h"
#include "soil.h"


TabHorizons::TabHorizons()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QLabel* dbTableLabel = new QLabel("Soil parameters from DB:");
    dbTableLabel->setStyleSheet("font: 11pt;");
    QLabel* modelTableLabel = new QLabel("Soil parameters estimated by model:");
    modelTableLabel->setStyleSheet("font: 11pt;");
    tableDb = new TableDbOrModel(dbTable);
    tableModel = new TableDbOrModel(modelTable);
    QHBoxLayout *addDeleteRowLayout = new QHBoxLayout;
    addRow = new QPushButton("+");
    addRow->setFixedWidth(40);
    deleteRow = new QPushButton("-");
    deleteRow->setFixedWidth(40);
    addRow->setEnabled(false);
    deleteRow->setEnabled(false);
    addDeleteRowLayout->addStretch(80);
    addDeleteRowLayout->addWidget(addRow);
    addDeleteRowLayout->addWidget(deleteRow);


    connect(tableDb->verticalHeader(), &QHeaderView::sectionClicked, [=](int index){ this->tableDbVerticalHeaderClick(index); });
    connect(tableModel->verticalHeader(), &QHeaderView::sectionClicked, [=](int index){ this->tableModelVerticalHeaderClick(index); });
    connect(addRow, &QPushButton::clicked, [=](){ this->addRowClicked(); });
    connect(deleteRow, &QPushButton::clicked, [=](){ this->removeRowClicked(); });

    mainLayout->addWidget(dbTableLabel);
    mainLayout->addWidget(tableDb);
    mainLayout->addLayout(addDeleteRowLayout);
    mainLayout->addWidget(modelTableLabel);
    mainLayout->addWidget(tableModel);

    setLayout(mainLayout);
}

void TabHorizons::insertSoilHorizons(soil::Crit3DSoil *soil, soil::Crit3DTextureClass* textureClassList)
{
    //disable events otherwise setBackgroundColor call again cellChanged event
    tableDb->blockSignals(true);
    mySoil = soil;
    myTextureClassList = textureClassList;

    int row = mySoil->nrHorizons;
    tableDb->setRowCount(row);
    tableModel->setRowCount(row);
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

        checkHorizonData(i);
        checkMissingItem(i);
    }
    clearSelections();

    tableDb->blockSignals(false);
    addRow->setEnabled(true);
    deleteRow->setEnabled(false);

    connect(tableDb, &QTableWidget::cellChanged, [=](int row, int column){ this->cellChanged(row, column); });
    connect(tableDb, &QTableWidget::cellClicked, [=](int row, int column){ this->cellClicked(row, column); });
    connect(tableModel, &QTableWidget::cellClicked, [=](int row, int column){ this->cellClicked(row, column); });


}

void TabHorizons::checkHorizonData(int horizonNum)
{
    if (mySoil->horizon[horizonNum].dbData.upperDepth > mySoil->horizon[horizonNum].dbData.lowerDepth)
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
        if (horizonNum > 0 && mySoil->horizon[horizonNum].dbData.upperDepth != mySoil->horizon[horizonNum-1].dbData.lowerDepth)
        {
            tableDb->item(horizonNum,0)->setBackgroundColor(Qt::red);
        }
    }

    if (mySoil->horizon[horizonNum].dbData.sand + mySoil->horizon[horizonNum].dbData.silt + mySoil->horizon[horizonNum].dbData.clay != 100)
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

        return;
    }

    if (mySoil->horizon[horizonNum].dbData.coarseFragments < 0 || mySoil->horizon[horizonNum].dbData.coarseFragments > 100)
    {
        tableDb->item(horizonNum,5)->setBackgroundColor(Qt::red);
    }

    if (mySoil->horizon[horizonNum].dbData.organicMatter < 0 || mySoil->horizon[horizonNum].dbData.organicMatter > 100)
    {
        tableDb->item(horizonNum,6)->setBackgroundColor(Qt::red);
    }

    if (mySoil->horizon[horizonNum].dbData.bulkDensity < 0 || mySoil->horizon[horizonNum].dbData.bulkDensity > QUARTZ_DENSITY)
    {
        tableDb->item(horizonNum,7)->setBackgroundColor(Qt::red);
    }

    if (mySoil->horizon[horizonNum].dbData.kSat <= 0)
    {
        tableDb->item(horizonNum,8)->setBackgroundColor(Qt::red);
    }

    if (mySoil->horizon[horizonNum].dbData.thetaSat <= 0 || mySoil->horizon[horizonNum].dbData.thetaSat >= 1)
    {
        tableDb->item(horizonNum,9)->setBackgroundColor(Qt::red);
    }

    checkComputedValues(horizonNum);

}

void TabHorizons::checkMissingItem(int horizonNum)
{
    QString NODATAString = "-9999";
    for (int j = 0; j < tableDb->columnCount(); j++)
    {
        if (tableDb->item(horizonNum,j)->text().contains(NODATAString))
        {
            tableDb->item(horizonNum,j)->setBackgroundColor(Qt::yellow);
            tableDb->item(horizonNum,j)->setText("");
        }
    }

    for (int j = 0; j < tableModel->columnCount(); j++)
    {
        if (tableModel->item(horizonNum,j)->text().contains(NODATAString))
        {
            tableModel->item(horizonNum,j)->setBackgroundColor(Qt::red);
            tableModel->item(horizonNum,j)->setText("");
        }
    }

}

void TabHorizons::checkComputedValues(int horizonNum)
{
    if (mySoil->horizon[horizonNum].dbData.coarseFragments != mySoil->horizon[horizonNum].coarseFragments*100)
    {
        tableModel->item(horizonNum,1)->setBackgroundColor(Qt::yellow);
    }

    if (mySoil->horizon[horizonNum].dbData.organicMatter != mySoil->horizon[horizonNum].organicMatter*100)
    {
        tableModel->item(horizonNum,2)->setBackgroundColor(Qt::yellow);
    }

    if (mySoil->horizon[horizonNum].dbData.bulkDensity != mySoil->horizon[horizonNum].bulkDensity)
    {
        tableModel->item(horizonNum,3)->setBackgroundColor(Qt::yellow);
    }

    if (mySoil->horizon[horizonNum].dbData.kSat != mySoil->horizon[horizonNum].waterConductivity.kSat)
    {
        tableModel->item(horizonNum,4)->setBackgroundColor(Qt::yellow);
    }

    if (mySoil->horizon[horizonNum].dbData.thetaSat != mySoil->horizon[horizonNum].vanGenuchten.thetaS)
    {
        tableModel->item(horizonNum,5)->setBackgroundColor(Qt::yellow);
    }

}

void TabHorizons::clearSelections()
{
    tableDb->clearSelection();
    tableModel->clearSelection();
    deleteRow->setEnabled(false);
}

void TabHorizons::tableDbVerticalHeaderClick(int index)
{
    tableDb->horizontalHeader()->setHighlightSections(false);
    tableModel->selectRow(index);
    tableModel->horizontalHeader()->setHighlightSections(false);
    if (index == tableDb->rowCount()-1)
    {
        deleteRow->setEnabled(true);
    }
    else
    {
        deleteRow->setEnabled(false);
    }
}

void TabHorizons::tableModelVerticalHeaderClick(int index)
{
    tableModel->horizontalHeader()->setHighlightSections(false);
    tableDb->selectRow(index);
    tableDb->horizontalHeader()->setHighlightSections(false);
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

    qDebug() << "Cell at row: " << QString::number(row) << " column " << QString::number(column)<<" was changed.";
    tableModel->selectRow(row);
    QString data = tableDb->item(row, column)->text();
    data.replace(",", ".");

    // set new value
    switch (column) {
        case 0:
        {
            mySoil->horizon[row].dbData.upperDepth = data.toDouble();
            tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 0));
            break;
        }
        case 1:
        {
            mySoil->horizon[row].dbData.lowerDepth = data.toDouble();
            tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 0));
            break;
        }
        case 2:
        {
            mySoil->horizon[row].dbData.sand = data.toFloat();
            tableDb->item(row, column)->setText(QString::number(data.toFloat(), 'f', 1));
            break;
        }
        case 3:
        {
            mySoil->horizon[row].dbData.silt = data.toFloat();
            tableDb->item(row, column)->setText(QString::number(data.toFloat(), 'f', 1));
            break;
        }
        case 4:
        {
            mySoil->horizon[row].dbData.clay = data.toFloat();
            tableDb->item(row, column)->setText(QString::number(data.toFloat(), 'f', 1));
            break;
        }
        case 5:
        {
            mySoil->horizon[row].dbData.coarseFragments = data.toDouble();
            tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 1));
            break;
        }
        case 6:
        {
            mySoil->horizon[row].dbData.organicMatter = data.toDouble();
            tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 1));
            break;
        }
        case 7:
        {
            mySoil->horizon[row].dbData.bulkDensity = data.toDouble();
            tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 3));
            break;
        }
        case 8:
        {
            mySoil->horizon[row].dbData.kSat = data.toDouble();
            tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 3));
            break;
        }
        case 9:
        {
            mySoil->horizon[row].dbData.thetaSat = data.toDouble();
            tableDb->item(row, column)->setText(QString::number(data.toDouble(), 'f', 3));
            break;
        }
    }

    std::string errorString;
    soil::setHorizon(&(mySoil->horizon[row]), myTextureClassList, &errorString);

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

    // check new values and assign background color
    checkHorizonData(row);
    checkMissingItem(row);
    tableDb->blockSignals(false);
}

void TabHorizons::cellClicked(int row, int column)
{
    tableDb->selectRow(row);
    tableModel->selectRow(row);
}

void TabHorizons::addRowClicked()
{
    tableDb->blockSignals(true);
    int numRow = tableDb->rowCount();
    QString lowerDepth = tableDb->item(numRow-1, 1)->text();
    tableDb->insertRow(numRow);
    tableModel->insertRow(numRow);

    for (int j=0; j<tableDb->columnCount(); j++)
    {
        tableDb->setItem(numRow, j, new QTableWidgetItem());
        tableModel->setItem(numRow, j, new QTableWidgetItem());
    }
    tableDb->scrollToBottom();
    tableModel->scrollToBottom();
    deleteRow->setEnabled(true);
    tableDb->item(numRow, 0)->setText(lowerDepth);
    tableDb->selectRow(numRow);
    tableModel->selectRow(numRow);

    // LC inserire una funziona di addHorizon
    tableDb->blockSignals(false);

}

void TabHorizons::removeRowClicked()
{
    tableDb->blockSignals(true);
    clearSelections();
    tableDb->removeRow(tableDb->rowCount()-1);
    tableModel->removeRow(tableDb->rowCount()-1);
    // LC inserire una funzione di removeHorizon
    tableDb->blockSignals(false);
}
