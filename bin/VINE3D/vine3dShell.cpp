#include "shell.h"
#include "vine3DShell.h"


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

    // TODO:
    // check file
    // for each line of file:
        // QStringList argumentList = getArgumentList(line)
        // executeCommand(argumentList)

    if (scriptFileName == "")
    {
        myProject->logError("No script file provided");
        return false;
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

