#ifndef SHELL_H
#define SHELL_H

    #define MODE_GUI 0
    #define MODE_BATCH 1
    #define MODE_CONSOLE 2

    #include <string>
    #include <vector>

    bool attachOutputToConsole();
    void openNewConsole();
    std::vector<std::string> getCommandLine(std::string programName);
    bool executeSharedCommand(std::vector<std::string> command, bool* isExit);

    void sendEnterKey(void);
    bool isConsoleForeground();

#endif // SHELL_H
