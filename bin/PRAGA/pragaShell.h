#ifndef PRAGASHELL_H
#define PRAGASHELL_H

    #ifndef PRAGAPROJECT_H
        #include "pragaProject.h"
    #endif

    bool executePragaCommand(QStringList commandLine, PragaProject* myProject);
    bool executeCommand(QStringList commandLine, PragaProject* myProject, bool* isExit);
    bool pragaShell(PragaProject* myProject);
    bool pragaBatch(PragaProject* myProject, QString batchFileName);


#endif // PRAGASHELL_H
