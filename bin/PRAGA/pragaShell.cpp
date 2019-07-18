#include "pragaShell.h"
#include "shell.h"


bool executePragaCommand(QStringList commandLine, PragaProject* myProject)
{
    int nrArgs = commandLine.size();
    if (nrArgs == 0) return false;

    QString command = commandLine[0].toUpper();

    // specific Praga commands
    // ...

    return false;
}


bool executeCommand(QStringList commandLine, PragaProject* myProject, bool* isExit)
{
    if (commandLine.size() > 0)
    {
        if (! myProject->executeSharedCommand(commandLine, isExit))
        {
            if (! executePragaCommand(commandLine, myProject))
            {
                myProject->logError("This is not a valid PRAGA command.");
                return false;
            }
        }
    }
    return true;
}


bool pragaBatch(PragaProject* myProject, QString scriptFileName)
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

    #ifdef _WIN32
        // Send "enter" to release application from the console
        // This is a hack, but if not used the console doesn't know the application has
        // returned. The "enter" key only sent if the console window is in focus.
        if (isConsoleForeground()) sendEnterKey();
    #endif

    return true;
}


bool pragaShell(PragaProject* myProject)
{
    #ifdef _WIN32
        openNewConsole();
    #endif

    bool isExit = false;
    while (! isExit)
    {
        std::string commandLine = getCommandLine("PRAGA");
        QStringList argumentList = getArgumentList(commandLine);
        executeCommand(argumentList, myProject, &isExit);
    }

    return true;
}

