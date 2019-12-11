#ifndef DIALOGSELECTION_H
#define DIALOGSELECTION_H

    #include <QString>
    #include <QDateTime>
    #include "meteo.h"
    #include "project.h"

    #ifdef NETCDF
        #include "netcdfHandler.h"
    #endif

    class Project;

    QString editValue(QString windowsTitle, QString defaultValue);

    meteoVariable chooseColorScale();
    frequencyType chooseFrequency();

    meteoVariable chooseMeteoVariable(Project *project_);

    #ifdef NETCDF
        bool chooseNetCDFVariable(NetCDFHandler* netCDF, int *varId, QDateTime *firstDate, QDateTime *lastDate);
    #endif

#endif // DIALOGSELECTION_H
