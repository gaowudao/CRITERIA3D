#pragma comment(lib, "User32.lib")

#include "shell.h"
#include "project.h"
#include <iostream>
#include <sstream>
#include <QString>
#include <QStringList>

#ifdef _WIN32
    #include "Windows.h"
#endif

using namespace std;


bool attachOutputToConsole()
{
    #ifdef _WIN32
        HANDLE consoleHandleOut, consoleHandleIn, consoleHandleError;

        if (AttachConsole(ATTACH_PARENT_PROCESS))
        {
            // Redirect unbuffered STDOUT to the console
            consoleHandleOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (consoleHandleOut != INVALID_HANDLE_VALUE)
            {
                freopen("CONOUT$", "w", stdout);
                setvbuf(stdout, nullptr, _IONBF, 0);
            }
            else
            {
                return false;
            }

            // Redirect STDIN to the console
            consoleHandleIn = GetStdHandle(STD_INPUT_HANDLE);
            if (consoleHandleIn != INVALID_HANDLE_VALUE)
            {
                freopen("CONIN$", "r", stdin);
                setvbuf(stdin, nullptr, _IONBF, 0);
            }
            else
            {
                return false;
            }

            // Redirect unbuffered STDERR to the console
            consoleHandleError = GetStdHandle(STD_ERROR_HANDLE);
            if (consoleHandleError != INVALID_HANDLE_VALUE)
            {
                freopen("CONOUT$", "w", stderr);
                setvbuf(stderr, nullptr, _IONBF, 0);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            // Not a console application
            return false;
        }
    #endif

    return true;
}

bool isConsoleForeground()
{
    #ifdef _WIN32
        return (GetConsoleWindow() == GetForegroundWindow());
    #endif
}

void sendEnterKey(void)
{
    #ifdef _WIN32
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
    #endif
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


QStringList getArgList(string commandLine)
{
    string str;
    QStringList argList;

    istringstream stream(commandLine);
    while (stream >> str)
    {
        argList.append(QString::fromStdString(str));
    }

    return argList;
}


QStringList getCommandLine(string programName)
{
    string commandLine;

    cout << programName << ">";
    getline (cin, commandLine);

    return getArgList(commandLine);
}


bool Project::executeSharedCommand(QStringList commandLine, bool* isExit)
{
    int nrArgs = commandLine.size();
    if (nrArgs == 0) return false;

    QString command = commandLine[0].toUpper();

    if (command == "QUIT" || command == "EXIT")
    {
        // close project
        *isExit = true;
        return true;
    }
    else
    {
        // other shared commands
        // ...
    }


    return false;
}


