#include "pragaShell.h"
#include "shell.h"
#include "mainWindow.h"


QStringList getPragaCommandList()
{
    QStringList cmdList = getSharedCommandList();

    // praga commands
    cmdList.append("List    | ListCommands");

    return cmdList;
}


bool cmdList(PragaProject* myProject)
{
    QStringList list = getPragaCommandList();

    myProject->logInfoInfo("Available PRAGA Console commands:");
    myProject->logInfoInfo("(short  | long version)");
    for (int i = 0; i < list.size(); i++)
    {
        myProject->logInfoInfo(list[i]);
    }

    return true;
}


bool PragaProject::executePragaCommand(QStringList argumentList, bool* isCommandFound)
{
    *isCommandFound = false;
    if (argumentList.size() == 0) return false;

    QString command = argumentList[0].toUpper();

    if (command == "LIST" || command == "LISTCOMMANDS")
    {
        *isCommandFound = true;
        return cmdList(this);
    }
    else
    {
        // other specific Praga commands
        // ...
    }

    return false;
}


bool executeCommand(QStringList argumentList, PragaProject* myProject)
{
    if (argumentList.size() == 0) return false;
    bool isCommandFound, isExecuted;

    myProject->logInfoInfo(getTimeStamp(argumentList));

    isExecuted = myProject->executeSharedCommand(argumentList, &isCommandFound);
    if (isCommandFound) return isExecuted;

    isExecuted = myProject->executePragaCommand(argumentList, &isCommandFound);
    if (isCommandFound) return isExecuted;

    myProject->logInfoError("This is not a valid PRAGA command.");
    return false;
}


bool pragaBatch(PragaProject* myProject, QString scriptFileName)
{
    #ifdef _WIN32
        attachOutputToConsole();
    #endif

    myProject->logInfoInfo("\nPRAGA v0.1");
    myProject->logInfoInfo("Execute script: " + scriptFileName);

    if (scriptFileName == "")
    {
        myProject->logInfoError("No script file provided");
        return false;
    }

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

    while (! myProject->requestedExit)
    {
        QString commandLine = getCommandLine("PRAGA");
        if (commandLine != "")
        {
            QStringList argumentList = getArgumentList(commandLine);
            executeCommand(argumentList, myProject);
        }
    }

    return true;
}

