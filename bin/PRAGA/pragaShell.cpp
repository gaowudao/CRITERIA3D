#include "pragaShell.h"
#include "shell.h"



bool executePragaCommand(QStringList argList, PragaProject* myProject)
{
    int nrArgs = argList.size();
    if (nrArgs == 0) return false;

    // specific Praga commands
    // ...

    return false;
}


bool pragaShell(PragaProject* myProject)
{
    #ifdef _WIN32
        openNewConsole();
    #endif

    bool isExit = false;
    while (! isExit)
    {
        QStringList command = getCommandLine("PRAGA");
        if (command.size() > 0)
        {
            if (! myProject->executeCommand(command, &isExit))
            {
                if (! executePragaCommand(command, myProject))
                {
                    myProject->logError("This is not a valid PRAGA command.");
                }
            }
        }
    }

    return true;
}
