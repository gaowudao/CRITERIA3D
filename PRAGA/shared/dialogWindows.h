#ifndef DIALOGWINDOWS_H
#define DIALOGWINDOWS_H

    #include <QString>

    #include "meteo.h"

    class Project;

    QString editValue(QString windowsTitle, QString defaultValue);

    meteoVariable chooseColorScale();
    frequencyType chooseFrequency();

    bool chooseMeteoVariable(Project *project_);
    bool downloadMeteoData(Project *project_);

    #ifdef NETCDF
        bool chooseNetCDFVariable(int *varId, QDateTime *firstDate, QDateTime *lastDate);
    #endif

#endif // DIALOGWINDOWS_H
