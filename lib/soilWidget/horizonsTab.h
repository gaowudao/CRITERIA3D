#ifndef HORIZONSTAB_H
#define HORIZONSTAB_H

#include <QDialog>
#include <QSettings>
#include <QtWidgets>
#ifndef SOIL_H
    #include "soil.h"
#endif

class HorizonsTab : public QWidget
{
    Q_OBJECT

public:
    HorizonsTab();
    void fillTextEdit(QString soilCode, soil::Crit3DSoil mySoil);

private:
    QTextEdit soilTextEdit;
};

#endif // HORIZONSTAB_H
