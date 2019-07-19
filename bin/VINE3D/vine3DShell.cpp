#include <QFile>
#include <QTextStream>
#include "shell.h"
#include "vine3DShell.h"

bool cmdOpenVine3DProject(Vine3DProject* myProject, QStringList argumentList)
{
    //myProject.setEnvironment(batch);

    if (argumentList.size() == 0)
        return false;

    QString projectName = argumentList.at(1);

    if (! myProject->loadProject(projectName))
    {
        myProject->logError("Open project failed:\n " + projectName);
    }

    return true;
}

bool cmdRunModels(Vine3DProject* myProject, QStringList argumentList)
{
    //myProject.setEnvironment(batch);

    if (argumentList.size() == 0)
        return false;

    QDate today, firstDay;
    int nrDays, nrDaysForecast;

    if (argumentList.size() > 3)
    {
        nrDaysForecast = argumentList.at(3).toInt();
        nrDays = argumentList.at(2).toInt();
    }
    else if (argumentList.size() > 2)
    {
        nrDays = argumentList.at(2).toInt();
        nrDaysForecast = 9;
    }
    else
    {
        nrDays = 7;      //default: 1 week
        nrDaysForecast = 9;
    }

    today = QDate::currentDate();
    QDateTime lastDateTime = QDateTime(today);
    lastDateTime = lastDateTime.addDays(nrDaysForecast);
    lastDateTime.setTime(QTime(23,0,0,0));

    firstDay = today.addDays(-nrDays);
    QDateTime firstDateTime = QDateTime(firstDay);
    firstDateTime.setTime(QTime(1,0,0,0));

    myProject->runModels(firstDateTime, lastDateTime, true, true, myProject->idArea);

    myProject->closeProject();

    return true;
}

bool executeCommand(QStringList commandLine, Vine3DProject* myProject)
{
    if (commandLine.size() == 0) return false;

    bool isCommandFound, isExecuted;

    isExecuted = myProject->executeSharedCommand(commandLine, &isCommandFound);
    if (isCommandFound) return isExecuted;

    isExecuted = myProject->executeVine3DCommand(commandLine, &isCommandFound);
    if (isCommandFound) return isExecuted;

    myProject->logError("This is not a valid PRAGA command.");
    return false;
}


bool vine3dBatch(Vine3DProject *myProject, QString scriptFileName)
{
    #ifdef _WIN32
        attachOutputToConsole();
    #endif

    myProject->logInfo("\nVINE3D v1.0");
    myProject->logInfo("Execute script: " + scriptFileName);

    if (scriptFileName == "")
    {
        myProject->logError("No script file provided");
        return false;
    }

    if (! QFile(scriptFileName).exists())
    {
        myProject->logError("Script file not found: " + scriptFileName);
        return false;
    }

    QString line;
    QStringList commandLine;

    QFile inputFile(scriptFileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          line = in.readLine();
          commandLine = line.split(" ");

          if (! executeCommand(commandLine, myProject))
          {
              inputFile.close();
              return false;
          }
       }
       inputFile.close();
    }

    #ifdef _WIN32
        // Send "enter" to release application from the console
        // This is a hack, but if not used the console doesn't know the application has
        // returned. The "enter" key only sent if the console window is in focus.
        if (isConsoleForeground()) sendEnterKey();
    #endif

    return true;
}


bool vine3dShell(Vine3DProject* myProject)
{
    #ifdef _WIN32
        openNewConsole();
    #endif

    while (! myProject->requestedExit)
    {
        QString commandLine = getCommandLine("VINE3D");
        if (commandLine != "")
        {
            myProject->logInfo(">> " + commandLine);
            QStringList argumentList = getArgumentList(commandLine);
            executeCommand(argumentList, myProject);
        }
    }

    return true;
}

