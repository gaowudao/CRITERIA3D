#ifndef PROJECT
#define PROJECT

#include <fstream>
class QString;

class Project
{

public:
    bool isProjectLoaded;

    QString path;
    QString name;
    QString configFileName;
    QString logFileName;

    QString errorString;

    std::ofstream logFile;
    std::ofstream outputFile;

    Project();

    void initialize();
    void closeProject();
    bool setLogFile();
    bool setOutputFile();

    virtual bool initializeProject(QString settingsFileName);
    virtual bool readSettings();

    void logInfo(QString myLog);
    void logError();
    void logError(QString myErrorStr);
    void writeOutput(QString myOut);
};


#endif // PROJECT

