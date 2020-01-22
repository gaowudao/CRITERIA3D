#include "dbfNumericFieldsDialog.h"

DbfNumericFieldsDialog::DbfNumericFieldsDialog(Crit3DShapeHandler* shapeHandler, QString fileName, bool isRasterize)
    :shapeHandler(shapeHandler)
{

    this->setWindowTitle("Choose a field of " + fileName);
    this->setFixedSize(400,300);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    listFields = new QListWidget();
    mainLayout->addWidget(listFields);

    if (isRasterize)
    {
        cellSize = new QLineEdit();
        cellSize->setPlaceholderText("cell size [m]");
        cellSize->setValidator(new QDoubleValidator(0, 9999.0, 2)); //LC accetta double con 2 cifre decimali da 0 a 9999
        outputName = new QLineEdit();
        outputName->setPlaceholderText("Output Name");
        mainLayout->addWidget(cellSize);
        mainLayout->addWidget(outputName);
    }

    DBFFieldType typeField;
    QStringList fields;

    fields << "Shape ID";
    for (int i = 0; i < shapeHandler->getFieldNumbers(); i++)
    {
        typeField = shapeHandler->getFieldType(i);
        if (typeField != FTString)
        {
            fields << QString::fromStdString(shapeHandler->getFieldName(i));
        }
    }
    listFields->addItems(fields);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);

    if (isRasterize)
        connect(buttonBox, &QDialogButtonBox::accepted, [=](){ this->acceptRasterize(); });
    else
        connect(buttonBox, &QDialogButtonBox::accepted, [=](){ this->acceptSelection(); });

    connect(buttonBox, &QDialogButtonBox::rejected, [=](){ QDialog::done(QDialog::Rejected); });

    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    exec();
}


DbfNumericFieldsDialog::~DbfNumericFieldsDialog()
{

}

void DbfNumericFieldsDialog::acceptRasterize()
{
    QListWidgetItem * itemSelected = listFields->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No items selected", "Select a field");
        return;
    }
    if (outputName->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Empty name", "Insert output name");
        return;
    }
    QDialog::done(QDialog::Accepted);
}


void DbfNumericFieldsDialog::acceptSelection()
{
    QListWidgetItem * itemSelected = listFields->currentItem();
    if (itemSelected == nullptr)
    {
        QMessageBox::information(nullptr, "No items selected", "Select a field");
        return;
    }

    QDialog::done(QDialog::Accepted);
}


QString DbfNumericFieldsDialog::getOutputName()
{
    return outputName->text();
}

double DbfNumericFieldsDialog::getCellSize() const
{
    QString cellString = cellSize->text();
    cellString.replace(",", ".");
    return cellString.toDouble();
}

QString DbfNumericFieldsDialog::getFieldSelected()
{
    return listFields->currentItem()->text();
}
