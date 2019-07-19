#ifndef TABHORIZONS_H
#define TABHORIZONS_H

#include <QtWidgets>
#ifndef SOIL_H
    #include "soil.h"
#endif

class TabHorizons : public QWidget
{
    Q_OBJECT

public:
    TabHorizons();
    void insertSoilHorizons(soil::Crit3DSoil mySoil);
    void checkHorizonDBData(soil::Crit3DSoil mySoil, int horizonNum);
    void checkMissingItem(int horizonNum);
    void checkComputedValues(soil::Crit3DSoil mySoil, int horizonNum);
    void clearSelections();
    void tableDbVerticalHeaderClick(int index);
    void tableModelVerticalHeaderClick(int index);

private:
    QTableWidget* tableDb;
    QTableWidget* tableModel;
};

#endif // HORIZONSTAB_H
