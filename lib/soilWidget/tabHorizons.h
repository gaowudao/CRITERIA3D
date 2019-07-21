#ifndef TABHORIZONS_H
#define TABHORIZONS_H

#include <QtWidgets>
#ifndef SOIL_H
    #include "soil.h"
#endif

#ifndef TABLEDB_H
#include "tableDbOrModel.h"
#endif

class TabHorizons : public QWidget
{
    Q_OBJECT

public:
    TabHorizons();
    void insertSoilHorizons(soil::Crit3DSoil mySoil);
    void checkHorizonData(soil::Crit3DSoil mySoil, int horizonNum);
    void checkMissingItem(int horizonNum);
    void checkComputedValues(soil::Crit3DSoil mySoil, int horizonNum);
    void clearSelections();
    void tableDbVerticalHeaderClick(int index);
    void tableModelVerticalHeaderClick(int index);
    void cellChanged(int row, int column);
    void cellClicked(int row, int column);

private:
    TableDbOrModel* tableDb;
    TableDbOrModel* tableModel;
};

#endif // HORIZONSTAB_H
