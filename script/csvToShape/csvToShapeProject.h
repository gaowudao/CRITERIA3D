#ifndef CSVTOSHAPEPROJECT_H
#define CSVTOSHAPEPROJECT_H

#include <QString>
#include <QFile>
#include "logger.h"

#define ERROR_SETTINGS_MISSING -1
#define ERROR_SETTINGS_WRONGFILENAME -2
#define ERROR_SETTINGS_MISSINGDATA -3

class CsvToShapeProject
{
public:
    bool isProjectLoaded;
    QString UCM;
    QString csv_data;
    QString csv_format;
    QString output_shape;
    QString logFileName;
    Logger logger;
    CsvToShapeProject();  
    void initialize();
    void closeProject();
    int initializeProject(QString settingsFileName);
};

#endif // CSVTOSHAPEPROJECT_H
