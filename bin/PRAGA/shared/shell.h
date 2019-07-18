#ifndef SHELL_H
#define SHELL_H

    #include <string>
    class QStringList;
    class Project;

    bool attachOutputToConsole();
    void openNewConsole();
    void sendEnterKey(void);
    bool isConsoleForeground();

    QStringList getArgumentList(std::string commandLine);
    std::string getCommandLine(std::string programName);
    QStringList getSharedCommandList();

    bool cmdLoadDEM(Project* myProject, QStringList argumentList);
    bool cmdExit(Project* myProject);


#endif // SHELL_H
