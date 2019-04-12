#include "newColDialog.h"

NewColDialog::NewColDialog()
{
    this->setWindowTitle("New Column");
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QLineEdit* name = new QLineEdit();
    name->setText("Insert name: ");
    QLineEdit* type = new QLineEdit();
    type->setText("Insert type: ");
    QLineEdit* value = new QLineEdit();
    value->setText("Insert default value: ");

    mainLayout->addWidget(name);
    mainLayout->addWidget(type);
    mainLayout->addWidget(value);

    this->setLayout(mainLayout);
    show();
}
