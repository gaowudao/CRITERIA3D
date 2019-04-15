#include "newColDialog.h"

NewColDialog::NewColDialog()
{

    this->setFixedSize(400,300);
    this->setWindowTitle("New Column");
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QVBoxLayout *vbox = new QVBoxLayout;

    name = new QLineEdit();
    name->setPlaceholderText("Insert name: ");

    QGroupBox *groupBox = new QGroupBox(tr("Insert type"));
    stringButton = new QRadioButton(tr("FTString"));
    intButton = new QRadioButton(tr("FTInteger"));
    doubleButton = new QRadioButton(tr("FTDouble"));
    stringButton->setChecked(true);


    nWidth = new QLineEdit();
    nWidth->setPlaceholderText("Insert the width of the field: ");

    nDecimals = new QLineEdit();
    nDecimals->setPlaceholderText("Insert the number of decimal: ");
    nDecimals->setVisible(false);

    mainLayout->addWidget(name);

    vbox->addWidget(stringButton);
    vbox->addWidget(intButton);
    vbox->addWidget(doubleButton);


    groupBox->setLayout(vbox);
    mainLayout->addWidget(groupBox);

    mainLayout->addWidget(nWidth);
    mainLayout->addWidget(nDecimals);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                             | QDialogButtonBox::Cancel);

    connect(stringButton, &QRadioButton::clicked, [=](){ this->hideDecimalEdit(); });
    connect(intButton, &QRadioButton::clicked, [=](){ this->showDecimalEdit(); });
    connect(doubleButton, &QRadioButton::clicked, [=](){ this->showDecimalEdit(); });
    connect(buttonBox, &QDialogButtonBox::accepted, [=](){ this->insertCol(); });
    connect(buttonBox, &QDialogButtonBox::rejected, [=](){ QDialog::done(QDialog::Rejected); });

    mainLayout->addWidget(buttonBox);
    this->setLayout(mainLayout);
    show();
}

void NewColDialog::insertCol()
{
    return;
}

void NewColDialog::showDecimalEdit()
{
    nDecimals->setVisible(true);
    return;
}

void NewColDialog::hideDecimalEdit()
{
    nDecimals->setVisible(false);
    return;
}
