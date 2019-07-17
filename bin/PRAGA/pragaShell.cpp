#include "pragaShell.h"
#include <iostream>

using namespace std;


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
    openNewConsole();

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
                    cout << "This is not a valid PRAGA command.\n";
                }
            }
        }
    }

    return true;
}
