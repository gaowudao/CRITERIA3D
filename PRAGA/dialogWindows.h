#ifndef DIALOGWINDOWS_H
#define DIALOGWINDOWS_H

    #include <QString>
    #include <QSettings>
    #include <QGridLayout>
    #include <QComboBox>

    #include <QtWidgets>

    #include "color.h"
    #include "meteo.h"

    QString editValue(QString windowsTitle, QString defaultValue);

    meteoVariable chooseColorScale();
    frequencyType chooseFrequency();

    bool chooseMeteoVariable();
    bool setInterpolationSettings();

    bool chooseNetCDFVariable(int *varId, QDateTime *firstDate, QDateTime *lastDate);

    bool downloadMeteoData();


#endif // DIALOGWINDOWS_H
