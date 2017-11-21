#ifndef CREEKPROJECT_H
#define CREEKPROJECT_H

#include "project.h"


class CreekProject : public Project
{
    void initializeCreek();
    bool readSettings();

public:
    QString WHCdata;
    QString precData;

    CreekProject();

    bool initializeProject(QString settingsFileName);

    void logInfoList(QStringList logList);
};




#endif // CREEKPROJECT_H
