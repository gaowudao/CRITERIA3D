#ifndef TABHORIZONS_H
#define TABHORIZONS_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef SOILTABLE_H
        #include "soilTable.h"
    #endif

    #include <QtWidgets>

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
        void resetAll();

    private:
        Crit3DSoilTable* tableDb;
        Crit3DSoilTable* tableModel;
        QPushButton* addRow;
        QPushButton* deleteRow;
        soil::Crit3DSoil* mySoil;
        soil::Crit3DTextureClass* myTextureClassList;
        std::string soilCodeChanged;

    signals:
        void horizonSelected(int nHorizon);

    };

#endif // HORIZONSTAB_H
