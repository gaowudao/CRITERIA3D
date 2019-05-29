#include "dbfNumericFieldsDialog.h"

DbfNumericFieldsDialog::DbfNumericFieldsDialog(Crit3DShapeHandler* shapeHandler, QString fileName)
    :shapeHandler(shapeHandler)
{

    this->setWindowTitle(fileName);
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


    setLayout(mainLayout);
    exec();
}


DbfNumericFieldsDialog::~DbfNumericFieldsDialog()
{

}
