#ifndef SHELL_H
#define SHELL_H

    #include <string>
    class QStringList;

    bool attachOutputToConsole();
    void openNewConsole();
    void sendEnterKey(void);
    bool isConsoleForeground();

    QStringList getArgumentList(std::string commandLine);
    std::string getCommandLine(std::string programName);
    QStringList getSharedCommandList();


#endif // SHELL_H
