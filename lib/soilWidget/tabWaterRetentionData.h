#ifndef TABWATERRETENTIONDATA_H
#define TABWATERRETENTIONDATA_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif

    #include <QtWidgets>

    class TabWaterRetentionData : public QWidget
    {
        Q_OBJECT
    public:
        TabWaterRetentionData();
        void insertData(soil::Crit3DSoil *soil);
        void tableVerticalHeaderClick(int index);
        void addRowClicked();
        void removeRowClicked();
        void resetAll();
        void cellClicked(int row, int column);
        void cellChanged(int row, int column);
        int moveRow(int row, int horizon, double waterPotential, double waterContent);

    private:
        QTableWidget* tableWaterRetention;
        QPushButton* addRow;
        QPushButton* deleteRow;
        soil::Crit3DSoil* mySoil;
        std::string soilCodeChanged;
        QVector<int> lastPositionHorizon;
        bool waterPotentialChanged;

    };


#endif // WATERRETENTIONDATA_H
