#ifndef SHELL_H
#define SHELL_H

    #include <string>
    class QString;
    class QStringList;

    bool attachOutputToConsole();
    void openNewConsole();
    void sendEnterKey(void);
    bool isConsoleForeground();
    QStringList getArgList(std::string commandLine);
    QStringList getCommandLine(std::string programName);


#endif // SHELL_H
