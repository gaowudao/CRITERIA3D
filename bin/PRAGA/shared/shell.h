#ifndef SHELL_H
#define SHELL_H

    class QString;
    class QStringList;

    bool attachOutputToConsole();
    void openNewConsole();
    void sendEnterKey(void);
    bool isConsoleForeground();

    QStringList getCommandLine(QString programName);

#endif // SHELL_H
