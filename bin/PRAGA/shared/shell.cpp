#pragma comment(lib, "User32.lib")
#include <iostream>
#include <sstream>
#include "windows.h"
#include "shell.h"

using namespace std;


bool attachOutputToConsole()
{
    HANDLE consoleHandleOut, consoleHandleError;

    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        // Redirect unbuffered STDOUT to the console
        consoleHandleOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (consoleHandleOut != INVALID_HANDLE_VALUE) {
            freopen("CONOUT$", "w", stdout);
            setvbuf(stdout, NULL, _IONBF, 0);
        }
        else {
            return FALSE;
        }

        // Redirect unbuffered STDERR to the console
        consoleHandleError = GetStdHandle(STD_ERROR_HANDLE);
        if (consoleHandleError != INVALID_HANDLE_VALUE) {
            freopen("CONOUT$", "w", stderr);
            setvbuf(stderr, NULL, _IONBF, 0);
        }
        else {
            return FALSE;
        }

        return TRUE;
    }

    //Not a console application
    return FALSE;
}

bool isConsoleForeground()
{
    return (GetConsoleWindow() == GetForegroundWindow());
}

void sendEnterKey(void)
{
    INPUT ip;
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Send the "Enter" key
    ip.ki.wVk = 0x0D; // virtual-key code for the "Enter" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the "Enter" key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

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


