#include "tabWaterRetentionData.h"
#include "tableDelegate.h"
#include "commonConstants.h"


TabWaterRetentionData::TabWaterRetentionData()
{
    QVBoxLayout* mainLayout = new QVBoxLayout;
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
    int margin = 25; // now table is empty
    tableWaterRetention->setFixedWidth(tableWaterRetention->horizontalHeader()->length() + tableWaterRetention->verticalHeader()->width() + margin);
    tableWaterRetention->setItemDelegate(new TableDelegate(tableWaterRetention));

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

    mainLayout->addWidget(tableWaterRetention);
    mainLayout->addLayout(addDeleteRowLayout);
    setLayout(mainLayout);
}

void TabWaterRetentionData::insertData(soil::Crit3DSoil *soil)
{
    if (soil == nullptr)
    {
        return;
    }
    tableWaterRetention->clearSelection();
    resetAll();
    deleteRow->setEnabled(false);
    mySoil = soil;
    int row = 0;
    for (int i = 0; i < mySoil->nrHorizons; i++)
    {
        if (mySoil->horizon[i].dbData.waterRetention.size() != 0)
        {
            row = row + mySoil->horizon[i].dbData.waterRetention.size();
            lastPositionHorizon << row-1;
            qDebug() << "lastPositionHorizon " << lastPositionHorizon[i];
        }

    }
    tableWaterRetention->setRowCount(row);
    int pos = 0;
    for (int i = 0; i < mySoil->nrHorizons; i++)
    {
        for (int j = 0; j < mySoil->horizon[i].dbData.waterRetention.size(); j++)
        {
            tableWaterRetention->setItem(pos, 0, new QTableWidgetItem( QString::number(i+1, 'f', 0)));
            tableWaterRetention->setItem(pos, 1, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.waterRetention[j].water_potential, 'f', 3)));
            tableWaterRetention->setItem(pos, 2, new QTableWidgetItem( QString::number(mySoil->horizon[i].dbData.waterRetention[j].water_content, 'f', 3 )));
            pos = pos + 1;
        }
    }
    connect(tableWaterRetention, &QTableWidget::cellClicked, [=](int row, int column){ this->cellClicked(row, column); });
    connect(tableWaterRetention, &QTableWidget::cellChanged, [=](int row, int column){ this->cellChanged(row, column); });
    addRow->setEnabled(true);
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
    int numRow;
    if (!tableWaterRetention->selectedItems().isEmpty())
    {
        if (tableWaterRetention->selectedItems().size() != tableWaterRetention->columnCount())
        {
            QMessageBox::critical(nullptr, "Warning", "Select the row of the horizon before the one you want to add");
            return;
        }
        numRow = tableWaterRetention->selectedItems().at(0)->row()+1;
    }
    else
    {
        numRow = tableWaterRetention->rowCount();
    }

    tableWaterRetention->insertRow(numRow);

    for (int j=0; j<tableWaterRetention->columnCount(); j++)
    {
        tableWaterRetention->setItem(numRow, j, new QTableWidgetItem());
    }

    tableWaterRetention->scrollToBottom();
    deleteRow->setEnabled(true);
    // TO DO
    soilCodeChanged = mySoil->code;


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
    int horizonSelected = tableWaterRetention->item(row,0)->text().toInt() - 1;
    int pos = row;
    if (horizonSelected != 0)
    {
        pos = pos - (lastPositionHorizon[horizonSelected-1]) -1;
    }
//    debug
//    qDebug() << "row " << row;
//    qDebug() << "pos " << pos;
//    qDebug() << "mySoil->horizon[horizon].dbData.waterRetention[pos].water_potential" << QString::number(mySoil->horizon[horizonSelected].dbData.waterRetention[pos].water_potential, 'f', 3);
//    qDebug() << "mySoil->horizon[horizon].dbData.waterRetention[pos].water_content" << QString::number(mySoil->horizon[horizonSelected].dbData.waterRetention[pos].water_content, 'f', 3);
    mySoil->horizon[horizonSelected].dbData.waterRetention.erase(mySoil->horizon[horizonSelected].dbData.waterRetention.begin()+pos);
    tableWaterRetention->removeRow(row);
    soilCodeChanged = mySoil->code;
}

void TabWaterRetentionData::resetAll()
{
    tableWaterRetention->setRowCount(0);
    lastPositionHorizon.clear();
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

    if (tableWaterRetention->itemAt(row,column) == nullptr || mySoil->nrHorizons < tableWaterRetention->item(row,0)->text().toInt())
    {
        QMessageBox::critical(nullptr, "Error!", "Add the horizont");
        return;
    }

    QString data = tableWaterRetention->item(row, column)->text();
    data.replace(",", ".");
    int horizon = tableWaterRetention->item(row,0)->text().toInt();

    // set new value
    switch (column) {
        case 0:
        {
            if (data == NODATA || data.isEmpty())
            {
                // TO DO
            }
            else
            {
                tableWaterRetention->item(row, column)->setText(QString::number(data.toDouble(), 'f', 0));
            }
            break;
        }
        case 1:
        {
            if (data == NODATA || data.isEmpty())
            {
                // TO DO
            }
            else
            {
                tableWaterRetention->item(row, column)->setText(QString::number(data.toDouble(), 'f', 3));
            }
            break;
        }
        case 2:
        {
            if (data == NODATA || data.isEmpty())
            {
                // TO DO
            }
            else
            {
                tableWaterRetention->item(row, column)->setText(QString::number(data.toFloat(), 'f', 3));
            }
            break;
        }

    }

    std::string errorString;
    // TO DO soil set new value

    tableWaterRetention->clearSelection();
    soilCodeChanged = mySoil->code;
}
