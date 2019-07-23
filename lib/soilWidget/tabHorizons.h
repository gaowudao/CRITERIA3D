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
    void insertSoilHorizons(soil::Crit3DSoil* mySoil, soil::Crit3DTextureClass *textureClassList);
    void checkHorizonData(int horizonNum);
    void checkMissingItem(int horizonNum);
    void checkComputedValues(int horizonNum);
    void clearSelections();
    void tableDbVerticalHeaderClick(int index);
    void tableModelVerticalHeaderClick(int index);
    void cellChanged(int row, int column);
    void cellClicked(int row, int column);
    void addRowClicked();
    void removeRowClicked();

private:
    TableDbOrModel* tableDb;
    TableDbOrModel* tableModel;
    QPushButton* addRow;
    QPushButton* deleteRow;
    soil::Crit3DSoil* mySoil;
    soil::Crit3DTextureClass* myTextureClassList;

};

#endif // HORIZONSTAB_H
