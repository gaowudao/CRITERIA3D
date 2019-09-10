#ifndef TABHORIZONS_H
#define TABHORIZONS_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef SOILTABLE_H
        #include "soilTable.h"
    #endif
    #ifndef BARHORIZON_H
        #include "barHorizon.h"
    #endif
    #include <QtWidgets>

    class TabHorizons : public QWidget
    {
        Q_OBJECT

    public:
        TabHorizons();
        void insertSoilHorizons(soil::Crit3DSoil* mySoil, soil::Crit3DTextureClass *textureClassList,
                                soil::Crit3DFittingOptions *fittingOptions);
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
        bool getSoilCodeChanged();
        void resetSoilCodeChanged();
        void resetAll();
        bool getInsertSoilElement() const;
        void setInsertSoilElement(bool value);

    private:
        Crit3DSoilTable* tableDb;
        Crit3DSoilTable* tableModel;
        BarHorizonList barHorizons;
        QPushButton* addRow;
        QPushButton* deleteRow;
        soil::Crit3DSoil* mySoil;
        soil::Crit3DTextureClass* myTextureClassList;
        soil::Crit3DFittingOptions* myFittingOptions;
        bool soilCodeChanged;
        bool insertSoilElement;
    private slots:
        void widgetClicked(int index);

    signals:
        void horizonSelected(int nHorizon);
        void updateSignal();


    };

#endif // TABHORIZONS_H
