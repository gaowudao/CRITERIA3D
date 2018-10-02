#ifndef DIALOGWINDOWS_H
#define DIALOGWINDOWS_H

    #include <QString>
    #include "color.h"
    #include "meteo.h"

    QString editValue(QString windowsTitle, QString defaultValue);

    meteoVariable chooseColorScale();
    frequencyType chooseFrequency();

    bool chooseMeteoVariable();

    #ifdef NETCDF
        bool chooseNetCDFVariable(int *varId, QDateTime *firstDate, QDateTime *lastDate);
    #endif

#endif // DIALOGWINDOWS_H
