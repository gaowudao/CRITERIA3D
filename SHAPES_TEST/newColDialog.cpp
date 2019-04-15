#include "newColDialog.h"

NewColDialog::NewColDialog()
{

    this->setFixedSize(400,250);
    this->setWindowTitle("New Column");
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QVBoxLayout *vbox = new QVBoxLayout;

    QLineEdit* name = new QLineEdit();
    name->setText("Insert name: ");

    QGroupBox *groupBox = new QGroupBox(tr("Insert type"));
    QRadioButton *stringButton = new QRadioButton(tr("FTString"));
    QRadioButton *intButton = new QRadioButton(tr("FTInteger"));
    QRadioButton *doubleButton = new QRadioButton(tr("FTDouble"));
    stringButton->setChecked(true);


    QLineEdit* nWidth = new QLineEdit();
    nWidth->setText("Insert the width of the field: ");

    QLineEdit* nDecimals = new QLineEdit();
    nDecimals->setText("Insert the number of decimal: ");
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
