#ifndef SHELL_H
#define SHELL_H

    #define MODE_GUI 0
    #define MODE_BATCH 1
    #define MODE_CONSOLE 2

    #include <string>
    #include <vector>

    void openNewConsole();
    std::vector<std::string> getCommandLine(std::string programName);
    bool executeSharedCommand(std::vector<std::string> command, bool* isExit);


#endif // SHELL_H
