#ifndef DIALOGWINDOWS_H
#define DIALOGWINDOWS_H

    #include <QString>
    #include "color.h"
    #include "meteo.h"

    QString editValue(QString windowsTitle, QString defaultValue);

    meteoVariable chooseColorScale();
    frequencyType chooseFrequency();

    bool chooseMeteoVariable();

    bool chooseNetCDFVariable(int *varId, QDateTime *firstDate, QDateTime *lastDate);

    bool downloadMeteoData();

    bool computation(QString title);

#endif // DIALOGWINDOWS_H
