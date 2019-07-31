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
    void checkDepths();
    bool checkHorizonData(int horizonNum);
    void checkMissingItem(int horizonNum);
    void checkComputedValues(int horizonNum);
    void setInvalidTableModelRow(int horizonNum);
    void clearSelections();
    void tableDbVerticalHeaderClick(int index);
    void editItem(int row, int column);
    void cellChanged(int row, int column);
    void cellClickedDb(int row, int column);
    void cellClickedModel(int row, int column);
    void addRowClicked();
    void removeRowClicked();
    std::string getSoilCodeChanged() const;
    void resetSoilCodeChanged();

private:
    TableDbOrModel* tableDb;
    TableDbOrModel* tableModel;
    QPushButton* addRow;
    QPushButton* deleteRow;
    soil::Crit3DSoil* mySoil;
    soil::Crit3DTextureClass* myTextureClassList;
    std::string soilCodeChanged;
signals:
    void horizonSelected(int nHorizon);

};

#endif // HORIZONSTAB_H
