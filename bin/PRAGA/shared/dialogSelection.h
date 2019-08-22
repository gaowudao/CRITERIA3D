#ifndef DIALOGSELECTION_H
#define DIALOGSELECTION_H

    #include <QString>

    #include "meteo.h"

    class Project;

    QString editValue(QString windowsTitle, QString defaultValue);

    meteoVariable chooseColorScale();
    frequencyType chooseFrequency();

    meteoVariable chooseMeteoVariable(Project *project_);

    #ifdef NETCDF
        bool chooseNetCDFVariable(int *varId, QDateTime *firstDate, QDateTime *lastDate);
    #endif

#endif // DIALOGSELECTION_H
