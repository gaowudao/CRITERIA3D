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
    void fillTextEdit(QString soilCode, soil::Crit3DSoil mySoil);

private:
    QTextEdit soilTextEdit;
    QTableWidget* tableDb;
    QTableWidget* tableModel;
};

#endif // HORIZONSTAB_H
