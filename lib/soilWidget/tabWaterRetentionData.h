#ifndef TABWATERRETENTIONDATA_H
#define TABWATERRETENTIONDATA_H

#include <QtWidgets>
#ifndef SOIL_H
    #include "soil.h"
#endif

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
private:
    QTableWidget* tableWaterRetention;
    QPushButton* addRow;
    QPushButton* deleteRow;
    soil::Crit3DSoil* mySoil;
};

#endif // WATERRETENTIONDATA_H
