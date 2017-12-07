#include <QString>
#include <QDir>
#include <QDateTime>
#include <QSettings>
#include <iostream>
#include <fstream>
#include "project.h"

using namespace std;


Project::Project() {}

void Project::initialize()
{
    this->isProjectLoaded = false;
    this->name = "";
    this->path = "";
    this->logFileName = "";
    this->configFileName = "";
    this->errorString = "";
}


void Project::closeProject()
{
    if (this->isProjectLoaded)
    {
        this->logInfo("Close Project...");
        this->logFile.close();

        this->isProjectLoaded = false;
    }
}


bool Project::initializeProject(QString settingsFileName)
{
    this->closeProject();
    this->initialize();

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

    if (!readSettings()) return false;

    this->setLogFile();
    this->setOutputFile();

    return true;
}


bool Project::readSettings()
{
    QSettings* projectSettings;
    projectSettings = new QSettings(this->configFileName, QSettings::IniFormat);

    projectSettings->beginGroup("project");

    path += projectSettings->value("path","").toString();
    name = projectSettings->value("name","").toString();

    projectSettings->endGroup();
    return true;
}


//-------------------
//
//   LOG functions
//
//-------------------
bool Project::setLogFile()
{
    if (!QDir(this->path + "log").exists())
         QDir().mkdir(this->path + "log");

    QString myDate = QDateTime().currentDateTime().toString("yyyy-MM-dd hh.mm");
    QString fileName = myDate + ".txt";

    this->logFileName = this->path + "log/" + fileName;
    this->logFile.open(this->logFileName.toStdString().c_str());

    logInfo("Create log file: " + this->logFileName);

    return (this->logFile.is_open());
}


void Project::logInfo(QString myLog)
{
    if (logFile.is_open())
        logFile << myLog.toStdString() << endl;

    cout << myLog.toStdString() << endl;
}


void Project::logError()
{
    if (logFile.is_open())
        logFile << "----ERROR!----" << endl << errorString.toStdString() << endl;;

    cout << "----ERROR!----" << endl << errorString.toStdString() << endl;;
}


void Project::logError(QString myError)
{
    errorString = myError;
    logError();
}


//---------------------
//
//   Output functions
//
//---------------------
bool Project::setOutputFile()
{
    if (!QDir(this->path + "output").exists())
         QDir().mkdir(this->path + "output");

    QString fileName = this->path + "output/" + this->name + ".csv";
    this->outputFile.open(fileName.toStdString().c_str());

    logInfo("Create output file: " + fileName);

    return (this->outputFile.is_open());
}


void Project::writeOutput(QString myOut)
{
    if (outputFile.is_open())
        outputFile << myOut.toStdString() << endl;

    cout << myOut.toStdString() << endl;
}

