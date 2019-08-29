#include "tabWaterRetentionData.h"
#include "tableDelegateWaterRetention.h"
#include "commonConstants.h"


TabWaterRetentionData::TabWaterRetentionData()
{
    QHBoxLayout* mainLayout = new QHBoxLayout;
    linesLayout = new QVBoxLayout;
    linesLayout->setAlignment(Qt::AlignHCenter);
    QVBoxLayout* tableLayout = new QVBoxLayout;
    QGroupBox *linesGroup = new QGroupBox(tr(""));
    linesGroup->setMinimumWidth(90);
    linesGroup->setTitle("Depth [cm]");
    tableWaterRetention = new QTableWidget();
    tableWaterRetention->setColumnCount(3);
    QStringList tableHeader;
    tableHeader << "Nr. horizon" << "Water potential [kPa]" << "Water content [m3 m-3]";
    tableWaterRetention->setHorizontalHeaderLabels(tableHeader);
    tableWaterRetention->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWaterRetention->resizeColumnsToContents();
    tableWaterRetention->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWaterRetention->setStyleSheet("QTableView::item:selected { color:black;  border: 3px solid black}");
    tableWaterRetention->setShowGrid(true);
    tableWaterRetention->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    int margin = 70; // now table is empty
    tableWaterRetention->setFixedWidth(tableWaterRetention->horizontalHeader()->length() + tableWaterRetention->verticalHeader()->width() + margin);
    tableWaterRetention->setItemDelegate(new TableDelegateWaterRetention(tableWaterRetention));
    tableWaterRetention->setSortingEnabled(true);
    tableWaterRetention->sortByColumn(0, Qt::AscendingOrder);

    QHBoxLayout *addDeleteRowLayout = new QHBoxLayout;
    QLabel* addDeleteLabel = new QLabel("Modify data:");
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

    connect(tableWaterRetention->verticalHeader(), &QHeaderView::sectionClicked, [=](int index){ this->tableVerticalHeaderClick(index); });
    connect(addRow, &QPushButton::clicked, [=](){ this->addRowClicked(); });
    connect(deleteRow, &QPushButton::clicked, [=](){ this->removeRowClicked(); });

    tableLayout->addWidget(tableWaterRetention);
    tableLayout->addLayout(addDeleteRowLayout);
    linesGroup->setLayout(linesLayout);
    mainLayout->addWidget(linesGroup);
    mainLayout->addLayout(tableLayout);
    setLayout(mainLayout);
    fillData = false;
    soilCodeChanged = false;
}

void TabWaterRetentionData::insertData(soil::Crit3DSoil *soil)
{

    QRect layoutSize = linesLayout->geometry();

    int totHeight = 0;

    // if layoutSize has no size (case tab in use)
    if (layoutSize.height() == 0)
    {
        totHeight = this->height() - (this->height() * 5 / 100);
    }
    else
    {
        totHeight = layoutSize.height();
    }

    if (soil == nullptr)
    {
        return;
    }
    resetAll();
    fillData = true;

    tableWaterRetention->blockSignals(true);
    tableWaterRetention->clearSelection();
    deleteRow->setEnabled(false);
    mySoil = soil;
    int row = 0;
    for (int i = 0; i < mySoil->nrHorizons; i++)
    {
        // insertVerticalLines
        int length = int((mySoil->horizon[i].lowerDepth*100 - mySoil->horizon[i].upperDepth*100) * totHeight / (mySoil->totalDepth*100));
        BarHorizons* line = new BarHorizons();
        line->setIndex(signed(i));
        line->setFixedWidth(25);
        line->setFixedHeight(length);
        line->setClass(mySoil->horizon[i].texture.classUSDA);
        linesLayout->addWidget(line);
        lineList.push_back(line);

        if (mySoil->horizon[i].dbData.waterRetention.size() != 0)
        {
            row = row + mySoil->horizon[i].dbData.waterRetention.size();
        }

    }
    tableWaterRetention->setRowCount(row);
    int pos = 0;
    tableWaterRetention->setSortingEnabled(false);
    for (int i = 0; i < mySoil->nrHorizons; i++)
    {
        for (int j = 0; j < mySoil->horizon[i].dbData.waterRetention.size(); j++)
        {
            tableWaterRetention->setItem(pos, 0, new QTableWidgetItem( QString::number(i+1, 'f', 0)));
            if (mySoil->horizon[i].dbData.waterRetention[j].water_potential < 1)
            {
                tableWaterRetention->setItem(pos, 1, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.waterRetention[j].water_potential, 'f', 3)));
            }
            else
            {
                tableWaterRetention->setItem(pos, 1, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.waterRetention[j].water_potential, 'f', 1)));
            }

            tableWaterRetention->setItem(pos, 2, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.waterRetention[j].water_content, 'f', 3 )));
            pos = pos + 1;
        }
    }
    tableWaterRetention->setSortingEnabled(true);
    if (!mySoil->code.empty())
    {
        addRow->setEnabled(true);
    }
    else
    {
        addRow->setEnabled(false);
    }
    tableWaterRetention->blockSignals(false);
    connect(tableWaterRetention, &QTableWidget::cellClicked, [=](int row, int column){ this->cellClicked(row, column); });
    connect(tableWaterRetention, &QTableWidget::cellChanged, [=](int row, int column){ this->cellChanged(row, column); });
    for (int i=0; i<lineList.size(); i++)
    {
        connect(lineList[i], SIGNAL(clicked(int)), this, SLOT(widgetClicked(int)));
    }

}

void TabWaterRetentionData::tableVerticalHeaderClick(int index)
{
    tableWaterRetention->setSelectionBehavior(QAbstractItemView::SelectRows);

    tableWaterRetention->selectRow(index);
    tableWaterRetention->horizontalHeader()->setHighlightSections(false);
    deleteRow->setEnabled(true);
}

void TabWaterRetentionData::addRowClicked()
{
    tableWaterRetention->setSortingEnabled(false);
    tableWaterRetention->blockSignals(true);
    int numRow;
    if (tableWaterRetention->rowCount() != 0)
    {
        if (tableWaterRetention->selectedItems().isEmpty())
        {
            QMessageBox::critical(nullptr, "Warning", "Select the row of the horizon before the one you want to add");
            return;
        }
        else
        {
            if (tableWaterRetention->selectedItems().size() != tableWaterRetention->columnCount())
            {
                QMessageBox::critical(nullptr, "Warning", "Select the row of the horizon before the one you want to add");
                return;
            }
            numRow = tableWaterRetention->selectedItems().at(0)->row()+1;
        }
    }
    else
    {
        numRow = 0;
    }

    tableWaterRetention->insertRow(numRow);

    // fill default row (copy the previous row)
    if (numRow == 0)
    {
        tableWaterRetention->setItem(numRow, 0, new QTableWidgetItem(QString::number(1)));
        tableWaterRetention->setItem(numRow, 1, new QTableWidgetItem(QString::number(0)));
        tableWaterRetention->setItem(numRow, 2, new QTableWidgetItem(QString::number(0)));
    }
    else
    {
        tableWaterRetention->setItem(numRow, 0, new QTableWidgetItem(tableWaterRetention->item(numRow-1,0)->text()));
        tableWaterRetention->setItem(numRow, 1, new QTableWidgetItem(tableWaterRetention->item(numRow-1,1)->text()));
        tableWaterRetention->setItem(numRow, 2, new QTableWidgetItem(tableWaterRetention->item(numRow-1,2)->text()));
    }
    tableWaterRetention->selectRow(numRow);
    if (!horizonChanged.contains(tableWaterRetention->item(numRow,0)->text().toInt()))
    {
        horizonChanged << tableWaterRetention->item(numRow,0)->text().toInt();
    }
    deleteRow->setEnabled(true);
    soilCodeChanged = true;

    tableWaterRetention->blockSignals(false);
    tableWaterRetention->setSortingEnabled(true);

}

void TabWaterRetentionData::removeRowClicked()
{
    int row;
    // check if a row is selected
    if (tableWaterRetention->selectedItems().isEmpty())
    {
        QMessageBox::critical(nullptr, "Error!", "Select a horizon");
        return;
    }
    if (tableWaterRetention->selectedItems().size() == tableWaterRetention->columnCount())
    {
        row = tableWaterRetention->selectedItems().at(0)->row();
    }
    else
    {
        QMessageBox::critical(nullptr, "Error!", "Select a horizon");
        return;
    }
    if (!horizonChanged.contains(tableWaterRetention->item(row,0)->text().toInt()))
    {
        horizonChanged << tableWaterRetention->item(row,0)->text().toInt();
    }

    tableWaterRetention->removeRow(row);
    soilCodeChanged = true;
}

void TabWaterRetentionData::resetAll()
{
    // delete all Widgets
    if (!lineList.isEmpty())
    {
        qDeleteAll(lineList);
        lineList.clear();
    }
    deleteRow->setEnabled(false);
    addRow->setEnabled(false);
    tableWaterRetention->clearContents();
    tableWaterRetention->setRowCount(0);
    tableWaterRetention->clearSelection();
    fillData = false;
    resetSoilCodeChanged();
}

void TabWaterRetentionData::cellClicked(int row, int column)
{

    tableWaterRetention->clearSelection();
    tableWaterRetention->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableWaterRetention->setItemSelected(tableWaterRetention->item(row,column), true);
    deleteRow->setEnabled(false);

}

void TabWaterRetentionData::cellChanged(int row, int column)
{

    if (tableWaterRetention->itemAt(row,column) == nullptr)
    {
        return;
    }
    tableWaterRetention->blockSignals(true);
    QString data = tableWaterRetention->item(row, column)->text();
    data.replace(",", ".");
    // set new value
    switch (column) {
        case 0:
        {
            if (data.toInt() <= 0 || round(data.toDouble()) > mySoil->nrHorizons)
            { 
                QMessageBox::critical(nullptr, "Error!", "Invalid horizon");
                if (row == 0)
                {
                    tableWaterRetention->item(row, column)->setText(QString::number(1));
                }
                else
                {
                    tableWaterRetention->item(row, column)->setText(tableWaterRetention->item(row-1,column)->text());
                }
                return;
            }
            else
            {
                tableWaterRetention->item(row, column)->setText(QString::number(round(data.toDouble())));
            }
            if (!horizonChanged.contains(data.toInt()))
            {
                horizonChanged << data.toInt();
            }
            break;
        }
        case 1:
        {
        // water potential
            if (data.toDouble() < 0)
            {
                QMessageBox::critical(nullptr, "Error!", "Insert valid water potential");
                if (row == 0)
                {
                    tableWaterRetention->item(row, column)->setText(QString::number(0));
                }
                else
                {
                    tableWaterRetention->item(row, column)->setText(tableWaterRetention->item(row-1,column)->text());
                }
                return;
            }
            else if (data.toDouble() < 1)
            {
                tableWaterRetention->item(row, column)->setText(QString::number(data.toDouble(), 'f', 3));
            }
            else
            {
                tableWaterRetention->item(row, column)->setText(QString::number(data.toDouble(), 'f', 1));
            }
            break;
        }
        // water content
        case 2:
        {
            if (data.toDouble() < 0 || data.toDouble() > 1)
            {
                QMessageBox::critical(nullptr, "Error!", "Insert valid water content");
                if (row == 0)
                {
                    tableWaterRetention->item(row, column)->setText(QString::number(0));
                }
                else
                {
                    tableWaterRetention->item(row, column)->setText(tableWaterRetention->item(row-1,column)->text());
                }
                return;
            }
            else
            {
                tableWaterRetention->item(row, column)->setText(QString::number(data.toFloat(), 'f', 3));
            }
            break;
        }

    }
    tableWaterRetention->sortByColumn(1, Qt::AscendingOrder);
    tableWaterRetention->sortByColumn(0, Qt::AscendingOrder);

    if (!horizonChanged.contains(tableWaterRetention->item(row,0)->text().toInt()))
    {
        horizonChanged << tableWaterRetention->item(row,0)->text().toInt();
    }

    tableWaterRetention->update();
    tableWaterRetention->blockSignals(false);
    soilCodeChanged = true;

}

void TabWaterRetentionData::updateSoil(soil::Crit3DSoil *soil)
{
    qSort(horizonChanged);
    soil::Crit3DWaterRetention waterRetention;
    for (int i = 0; i < horizonChanged.size(); i++)
    {
        //remove all prev points
        soil->horizon[horizonChanged[i]-1].dbData.waterRetention.erase(soil->horizon[horizonChanged[i]-1].dbData.waterRetention.begin(), soil->horizon[horizonChanged[i]-1].dbData.waterRetention.end());
        soil->horizon[horizonChanged[i]-1].dbData.waterRetention.clear();
        for (int j = 0; j < tableWaterRetention->rowCount(); j++)
        {
            if (tableWaterRetention->item(j,0)->text().toInt() == horizonChanged[i])
            {
                waterRetention.water_potential = tableWaterRetention->item(j,1)->text().toDouble();  // [kPa]
                waterRetention.water_content = tableWaterRetention->item(j,2)->text().toDouble();      // [m3 m-3]
                soil->horizon[horizonChanged[i]-1].dbData.waterRetention.push_back(waterRetention);
            }
        }
    }
}

bool TabWaterRetentionData::getSoilCodeChanged()
{
    return soilCodeChanged;
}

void TabWaterRetentionData::resetSoilCodeChanged()
{
    soilCodeChanged = false;
    horizonChanged.clear();
}

QVector<int> TabWaterRetentionData::getHorizonChanged() const
{
    return horizonChanged;
}

bool TabWaterRetentionData::getFillData() const
{
    return fillData;
}

void TabWaterRetentionData::setFillData(bool value)
{
    fillData = value;
}

void TabWaterRetentionData::widgetClicked(int index)
{
    // check selection state
    if (lineList[index]->getSelected())
    {
        // clear previous selection
        for(int i = 0; i < lineList.size(); i++)
        {
            if (i != index)
            {
                lineList[i]->restoreFrame();
                lineList[i]->setSelected(false);
            }
        }
    }
    else
    {
        tableWaterRetention->clearSelection();
    }

}
