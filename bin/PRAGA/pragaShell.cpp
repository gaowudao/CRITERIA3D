#include "pragaShell.h"
#include "shell.h"



bool executePragaCommand(QStringList argList, PragaProject* myProject)
{
    int nrArgs = argList.size();
    if (nrArgs == 0) return false;

    QString command = argList[0].toUpper();

    // specific Praga commands
    // ...

    return false;
}


bool executeCommand(QStringList argList, PragaProject* myProject, bool* isExit)
{
    if (argList.size() > 0)
    {
        if (! myProject->executeSharedCommand(argList, isExit))
        {
            if (! executePragaCommand(argList, myProject))
            {
                myProject->logError("This is not a valid PRAGA command.");
            }
        }
    }
    return true;
}


bool pragaBatch(PragaProject* myProject, QString batchFileName)
{

}


bool pragaShell(PragaProject* myProject)
{
    #ifdef _WIN32
        openNewConsole();
    #endif

    bool isExit = false;
    while (! isExit)
    {
        QStringList commandLine = getCommandLine("PRAGA");
        executeCommand(commandLine, myProject, &isExit);
    }

    return true;
}
