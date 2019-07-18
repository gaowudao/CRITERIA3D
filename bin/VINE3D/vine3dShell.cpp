#include "vine3dShell.h"
#include "shell.h"


bool executeVine3DCommand(QStringList argumentList, Vine3DProject* myProject)
{
    int nrArgs = argumentList.size();
    if (nrArgs == 0) return false;

    QString command = argumentList[0].toUpper();

    // specific Praga commands
    // ...

    return false;
}


bool executeCommand(QStringList commandLine, Vine3DProject* myProject)
{
    if (commandLine.size() > 0)
    {
        if (! myProject->executeSharedCommand(commandLine))
        {
            if (! executeVine3DCommand(commandLine, myProject))
            {
                myProject->logError("This is not a valid PRAGA command.");
                return false;
            }
        }
    }
    return true;
}


bool pragaBatch(Vine3DProject *myProject, QString scriptFileName)
{
    #ifdef _WIN32
        attachOutputToConsole();
    #endif

    myProject->logInfo("\nPRAGA v0.1");
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


bool pragaShell(Vine3DProject* myProject)
{
    #ifdef _WIN32
        openNewConsole();
    #endif

    while (! myProject->requestedExit)
    {
        std::string commandLine = getCommandLine("PRAGA");
        QStringList argumentList = getArgumentList(commandLine);
        executeCommand(argumentList, myProject);
    }

    return true;
}
