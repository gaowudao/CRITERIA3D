#include "shell.h"
#include <iostream>
#include <sstream>
#include <Windows.h>


using namespace std;


void openNewConsole()
{
    #ifdef _WIN32
        // detach from the current console window
        // if launched from a console window, that will still run waiting for the new console (below) to close
        // it is useful to detach from Qt Creator's <Application output> panel
        FreeConsole();

        // create a separate new console window
        AllocConsole();

        // attach the new console to this application's process
        AttachConsole(GetCurrentProcessId());

        // reopen the std I/O streams to redirect I/O to the new console
        freopen("CON", "w", stdout);
        freopen("CON", "w", stderr);
        freopen("CON", "r", stdin);
    #endif
}


vector<string> getCommandLine(string programName)
{
    vector<string> command;
    string str, commandLine;

    cout << programName << ">";
    getline (cin, commandLine);

    istringstream stream(commandLine);
    while (stream >> str) command.push_back(str);

    return command;
}


bool executeSharedCommand(vector<string> command, bool* isExit)
{
    int nrArgs = command.size();

    if (command[0] == "quit" || command[0] == "exit")
    {
        *isExit = true;
        return true;
    }

    // other shared commands
    // ...

    return false;
}


