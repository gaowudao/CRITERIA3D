#include "dbfNumericFieldsDialog.h"

DbfNumericFieldsDialog::DbfNumericFieldsDialog(Crit3DShapeHandler* shapeHandler, QString fileName)
    :shapeHandler(shapeHandler)
{

    this->setWindowTitle("Choose a field of " + fileName);
    this->setFixedSize(400,300);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    listFields = new QListWidget();
    mainLayout->addWidget(listFields);
    outputName = new QLineEdit();
    outputName->setPlaceholderText("Output Name");
    mainLayout->addWidget(outputName);

    std::string nameField;
    QStringList fieldsLabel;
    DBFFieldType typeField;

    fieldsLabel << "Shape ID";
    for (int i = 0; i < shapeHandler->getFieldNumbers(); i++)
    {
        typeField = shapeHandler->getFieldType(i);
        if (typeField != FTString)
        {
            nameField =  shapeHandler->getFieldName(i);
            fieldsLabel << QString::fromStdString(nameField);
        }
    }
    listFields->addItems(fieldsLabel);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, [=](){ this->fieldToRaster(); });
    connect(buttonBox, &QDialogButtonBox::rejected, [=](){ QDialog::done(QDialog::Rejected); });

    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    exec();
}


DbfNumericFieldsDialog::~DbfNumericFieldsDialog()
{

}

void DbfNumericFieldsDialog::fieldToRaster()
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

QString DbfNumericFieldsDialog::getOutputName()
{
    return outputName->text();
}

QString DbfNumericFieldsDialog::getFieldSelected()
{
    return listFields->currentItem()->text();
}
