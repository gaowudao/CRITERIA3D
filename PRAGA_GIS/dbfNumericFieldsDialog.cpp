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


    setLayout(mainLayout);
    exec();
}


DbfNumericFieldsDialog::~DbfNumericFieldsDialog()
{

}
