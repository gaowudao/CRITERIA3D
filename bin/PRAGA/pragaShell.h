#ifndef PRAGASHELL_H
#define PRAGASHELL_H

    #ifndef PRAGAPROJECT_H
        #include "pragaProject.h"
    #endif

    bool executePragaCommand(QStringList argumentList, PragaProject* myProject);
    bool executeCommand(QStringList commandLine, PragaProject* myProject);
    bool pragaShell(PragaProject* myProject);
    bool pragaBatch(PragaProject* myProject, QString batchFileName);


#endif // PRAGASHELL_H
