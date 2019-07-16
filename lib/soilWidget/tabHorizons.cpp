#include "tabHorizons.h"

TabHorizons::TabHorizons()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(&soilTextEdit);
    setLayout(mainLayout);
}

// example
void TabHorizons::fillTextEdit(QString soilCode, soil::Crit3DSoil mySoil)
{

    soilTextEdit.clear();
    // show data (example)
    soilTextEdit.append(soilCode);
    soilTextEdit.append("Horizon nr., sand (%),   silt (%),   clay (%)");
    for (int i = 0; i < mySoil.nrHorizons; i++)
    {
        QString s;
        s = QString::number(mySoil.horizon[i].dbData.horizonNr)
                + "\t" + QString::number(mySoil.horizon[i].dbData.sand)
                + "\t" + QString::number(mySoil.horizon[i].dbData.silt)
                + "\t" + QString::number(mySoil.horizon[i].dbData.clay);
        soilTextEdit.append(s);
    }

}
