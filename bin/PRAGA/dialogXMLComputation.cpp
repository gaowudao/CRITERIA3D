#include "dialogXMLComputation.h"

DialogXMLComputation::DialogXMLComputation(bool isAnomaly, QStringList listXML): isAnomaly(isAnomaly), listXML(listXML)
{
    if (isAnomaly)
    {
        setWindowTitle("Anomaly");
    }
    else
    {
        setWindowTitle("Elaboration");
    }
    QHBoxLayout mainLayout;
    QVBoxLayout elabLayout;
    QHBoxLayout layoutOk;

    listXMLWidget.addItems(listXML);
    elabLayout.addWidget(&listXMLWidget);

    connect(&listXMLWidget, &QListWidget::itemClicked, [=](QListWidgetItem* item){ this->elabClicked(item); });

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(&buttonBox, &QDialogButtonBox::accepted, [=](){ this->done(true); });
    connect(&buttonBox, &QDialogButtonBox::rejected, [=](){ this->done(false); });

    layoutOk.addWidget(&buttonBox);
    mainLayout.addLayout(&elabLayout);
    mainLayout.addLayout(&layoutOk);

    setLayout(&mainLayout);

    show();
    exec();
}

void DialogXMLComputation::elabClicked(QListWidgetItem* item)
{

}
