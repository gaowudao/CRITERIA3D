#include <QFile>
#include <QDir>
#include <QSettings>
#include <iostream>

#include "project.h"
#include "creekProject.h"

CreekProject::CreekProject() : Project()
{
    initializeCreek();
}


void CreekProject::initializeCreek()
{
    this->initialize();

    this->WHCdata = "";
    this->precData = "";
}


bool CreekProject::initializeProject(QString settingsFileName)
{
    this->closeProject();
    this->initializeCreek();

    if (settingsFileName == "")
    {
        this->logError("Missing settings File.");
        return false;
    }

    // Configuration file
    QFile myFile(settingsFileName);
    if (myFile.exists())
    {
        this->configFileName = QDir(myFile.fileName()).canonicalPath();
        this->configFileName = QDir().cleanPath(configFileName);
        this->logInfo("Using config file: " + configFileName);

        QFileInfo fileInfo(configFileName);
        this->path = fileInfo.path() + "/";
    }
    else
    {
        this->logError("Cannot find config file: " + settingsFileName);
        return false;
    }

    if (!readSettings()) return(false);

    //this->setLogFile();

    if (this->WHCdata == "")
    {
        this->logError("Wrong settings: missing WHC data");
        return false;
    }

    if (this->precData == "")
    {
        this->logError("Wrong settings: missing precipitation data");
        return false;
    }

    this->isProjectLoaded = true;

    return true;
}


bool CreekProject::readSettings()
{
    QSettings* projectSettings;
    projectSettings = new QSettings(this->configFileName, QSettings::IniFormat);

    projectSettings->beginGroup("project");

    path += projectSettings->value("path","").toString();
    name = projectSettings->value("name","").toString();

    WHCdata = projectSettings->value("WHCdata","").toString();
    if (WHCdata.left(1) == ".") WHCdata = path + WHCdata;

    precData = projectSettings->value("precData","").toString();
    if (precData.left(1) == ".") precData = path + precData;

    projectSettings->endGroup();
    return true;
}


void CreekProject::logInfoList(QStringList logList)
{
    for (int i = 0; i < logList.length(); i++)
    {
        std::cout << logList[i].toStdString();
        if (i < logList.length()-1)
            std::cout << ",";
        else
            std::cout << std::endl;
    }
}
