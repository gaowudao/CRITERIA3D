#include "pragaShell.h"
#include <vector>
#include <iostream>
#include <sstream>


using namespace std;


bool executePragaCommand(vector<string> command, PragaProject* myProject)
{
    int nrArgs = command.size();

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
        vector<string> command = getCommandLine("PRAGA");
        if (command.size() > 0)
        {
            if (! executeSharedCommand(command, &isExit))
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
