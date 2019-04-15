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
    nWidth->setValidator(new QIntValidator(0,100));

    nDecimals = new QLineEdit();
    nDecimals->setPlaceholderText("Insert the number of decimal: ");
    nDecimals->setVisible(false);
    nDecimals->setValidator(new QIntValidator(0,10));

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

bool NewColDialog::insertCol()
{
    if (!checkValidData())
    {
        return false;
    }
    return true;

}

bool NewColDialog::checkValidData()
{
    if (name->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing name", "Insert name");
        return false;
    }
    if (nWidth->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing width", "Insert the width of the field");
        return false;
    }
    if (intButton->isChecked() || doubleButton->isChecked())
    {
        if (nDecimals->text().isEmpty())
        {
            QMessageBox::information(nullptr, "Missing decimals", "Insert the number of decimal");
            return false;
        }
    }

    if (nWidth->text().contains(",") || nWidth->text().contains("+") || nWidth->text().contains("-"))
    {
        QMessageBox::information(nullptr, "Invalid width", "Insert only digits");
        return false;
    }
    if (nDecimals->text().contains(",") || nDecimals->text().contains("+") || nDecimals->text().contains("-"))
    {
        QMessageBox::information(nullptr, "Invalid decimals", "Insert only digits");
        return false;
    }
    return true;
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
