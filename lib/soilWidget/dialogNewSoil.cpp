#include "dialogNewSoil.h"

DialogNewSoil::DialogNewSoil()
{
    setWindowTitle("New Soil");
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QGridLayout *layoutSoil = new QGridLayout();
    QHBoxLayout *layoutOk = new QHBoxLayout();

    QLabel *idSoilLabel = new QLabel(tr("Enter soil ID (for the soil map): "));
    idSoilValue = new QLineEdit();

    QLabel *codeSoilLabel = new QLabel(tr("Enter soil code (string without spaces): "));
    codeSoilValue = new QLineEdit();

    QLabel *nameSoilLabel = new QLabel(tr("Enter soil name: "));
    nameSoilValue = new QLineEdit();

    layoutSoil->addWidget(idSoilLabel, 0 , 0);
    layoutSoil->addWidget(idSoilValue, 0 , 1);
    layoutSoil->addWidget(codeSoilLabel, 1 , 0);
    layoutSoil->addWidget(codeSoilValue, 1 , 1);
    layoutSoil->addWidget(nameSoilLabel, 2 , 0);
    layoutSoil->addWidget(nameSoilValue, 2 , 1);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(&buttonBox, &QDialogButtonBox::accepted, [=](){ this->done(true); });
    connect(&buttonBox, &QDialogButtonBox::rejected, [=](){ this->done(false); });

    layoutOk->addWidget(&buttonBox);


    mainLayout->addLayout(layoutSoil);
    mainLayout->addLayout(layoutOk);

    setLayout(mainLayout);

    show();
    exec();
}
