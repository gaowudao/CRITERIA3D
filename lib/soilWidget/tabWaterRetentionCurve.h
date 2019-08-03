#ifndef TABWATERRETENTIONCURVE_H
#define TABWATERRETENTIONCURVE_H

#include <QtWidgets>
#include "soil.h"
#include "lineHorizont.h"


class TabWaterRetentionCurve: public QWidget
{
    Q_OBJECT
public:
    TabWaterRetentionCurve();
    void insertVerticalLines(soil::Crit3DSoil* soil);
private:
    soil::Crit3DSoil* mySoil;
    QVBoxLayout* linesLayout;
    QList<LineHorizont*> lineList;

};

#endif // WATERRETENTIONCURVE_H
