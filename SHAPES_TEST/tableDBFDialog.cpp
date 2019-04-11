#include "tableDBFDialog.h"

tableDBFDialog::tableDBFDialog(Crit3DShapeHandler* shapeHandler)
    :shapeHandler(shapeHandler)
{
    QVBoxLayout* DBFLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;

    resize(QDesktopWidget().availableGeometry().size());

    m_DBFTableWidget = new QTableWidget();
    DBFLayout->addWidget(m_DBFTableWidget);


    int colNumber = shapeHandler->getFieldNumbers();
    int rowNumber = shapeHandler->getDBFRecordCount();
    m_DBFTableWidget->setRowCount(rowNumber);
    m_DBFTableWidget->setColumnCount(colNumber);
    QStringList m_DBFTableHeader;
    std::string nameField;
    int typeField;

    for (int i = 0; i < colNumber; i++)
    {
        nameField =  shapeHandler->getFieldName(i);
        typeField = shapeHandler->getFieldType(i);
        m_DBFTableHeader << QString::fromStdString(nameField);

        for (int j = 0; j < rowNumber; j++)
        {

            if (typeField == 0)
            {
                m_DBFTableWidget->setItem(j, i, new QTableWidgetItem( QString::fromStdString(shapeHandler->readStringAttribute(j,i) )));
            }
            else if (typeField == 1)
            {
                m_DBFTableWidget->setItem(j, i, new QTableWidgetItem( QString::number(shapeHandler->readIntAttribute(j,i) )));
            }
            else if (typeField == 2)
            {
                m_DBFTableWidget->setItem(j, i, new QTableWidgetItem( QString::number(shapeHandler->readDoubleAttribute(j,i) )));
            }
            if (shapeHandler->isDBFRecordDeleted(j))
            {
                m_DBFTableWidget->item(j,i)->setBackgroundColor(Qt::yellow);    // mark as DELETED records
            }


        }
    }

    labels.clear();
    for (int j = 0; j < rowNumber; j++)
    {
        labels << QString::number(j);
    }
    m_DBFTableWidget->setVerticalHeaderLabels(labels);
    m_DBFTableWidget->setHorizontalHeaderLabels(m_DBFTableHeader);
    //m_DBFTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_DBFTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_DBFTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_DBFTableWidget->setShowGrid(false);
    m_DBFTableWidget->setStyleSheet("QTableView {selection-background-color: red;}");

    int offset = 100;
    m_DBFTableWidget->setMinimumWidth(this->width());
    m_DBFTableWidget->setMaximumWidth(this->width());
    m_DBFTableWidget->setMinimumHeight(this->height() - offset);
    m_DBFTableWidget->setMaximumHeight(this->height() - offset);

    const QSize BUTTON_SIZE = QSize(22, 22);
    QLabel* labelLengend = new QLabel();
    labelLengend->setText("Deleted rows: yellow");
    m_addRowButton = new QPushButton();
    m_addRowButton->setText("+");
    m_addRowButton->setMaximumSize(BUTTON_SIZE);
    m_removeRowButton = new QPushButton();
    m_removeRowButton->setText("-");
    m_removeRowButton->setMaximumSize(BUTTON_SIZE);

    buttonLayout->addWidget(labelLengend);
    buttonLayout->addWidget(m_addRowButton);
    buttonLayout->addWidget(m_removeRowButton);
    DBFLayout->addLayout(buttonLayout);

    connect(m_DBFTableWidget, &QTableWidget::cellChanged, [=](int row, int column){ this->cellChanged(row, column); });
    connect(m_addRowButton, &QPushButton::clicked, [=](){ this->addRowClicked(); });
    connect(m_removeRowButton, &QPushButton::clicked, [=](){ this->removeRowClicked(); });


    setLayout(DBFLayout);
    show();

}


void tableDBFDialog::addRowClicked()
{

    m_DBFTableWidget->insertRow(m_DBFTableWidget->rowCount());
    labels << QString::number(labels.size());
    m_DBFTableWidget->setVerticalHeaderLabels(labels);


    // test
    //std::vector<std::string> test = {"0", "19850526",  "M0M0M0", "RAINFEDWHEAT" , "CTL4", "01139" , "01139" , "-9.99900e+003" , "7.3", "0", "0", "0", "0", "-9.99900e+003" , "-9.99900e+003"};
//    std::vector<std::string> test = {"6667"};
//    if (shapeHandler->addRecord(test))
//    {
//        qDebug() << "addRecord true ";
//    }
//    else
//    {
//        qDebug() << "addRecord false ";
//    }
}

void tableDBFDialog::removeRowClicked()
{
    qDebug() << "removeRowClicked ";
    QItemSelectionModel *select = m_DBFTableWidget->selectionModel();

    if (select->hasSelection())
    {
        QModelIndexList indexList = select->selectedRows();
        int row = indexList.at(0).row();

        if (shapeHandler->deleteRecord(row) || !indexList.at(0).data(Qt::DisplayRole).isValid())
        {
            qDebug() << "deleteRecord = " << row;
            m_DBFTableWidget->removeRow(row);
        }
    }
    else
    {
        qDebug() << "no row selected ";
    }

}

void tableDBFDialog::cellChanged(int row, int column)
{
    //qDebug() << "Cell at row: " << QString::number(row) << " column " << QString::number(column)<<" was changed.";
    QString data = m_DBFTableWidget->item(row, column)->text();
    int typeField = shapeHandler->getFieldType(column);
    if (typeField == 0)
    {
        shapeHandler->writeStringAttribute(row,column,data.toStdString().c_str());
    }
    else if (typeField == 1)
    {
        shapeHandler->writeIntAttribute(row,column, data.toInt());
    }
    else if (typeField == 2)
    {
        shapeHandler->writeDoubleAttribute(row,column, data.toDouble());
    }

}

void tableDBFDialog::closeEvent(QCloseEvent *event)
{
    shapeHandler->closeDBF();
    QDialog::closeEvent(event);
}


