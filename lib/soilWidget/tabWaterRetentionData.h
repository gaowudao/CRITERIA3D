#ifndef TABWATERRETENTIONDATA_H
#define TABWATERRETENTIONDATA_H

    #ifndef SOIL_H
        #include "soil.h"
    #endif
    #ifndef BARHORIZON_H
        #include "barHorizon.h"
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
        void updateSoil(soil::Crit3DSoil *soil);
        bool getSoilCodeChanged();
        void resetSoilCodeChanged();
        QVector<int> getHorizonChanged() const;
        bool getFillData() const;
        void setFillData(bool value);

    private:
        QTableWidget* tableWaterRetention;
        BarHorizonList barHorizons;
        QPushButton* addRow;
        QPushButton* deleteRow;
        soil::Crit3DSoil* mySoil;
        bool soilCodeChanged;
        QVector<int> horizonChanged;
        bool fillData;
    private slots:
        void widgetClicked(int index);

    };


#endif // WATERRETENTIONDATA_H
