#ifndef PRAGASHELL_H
#define PRAGASHELL_H

    #ifndef PRAGAPROJECT_H
        #include "pragaProject.h"
    #endif

    bool executePragaCommand(QStringList argList, PragaProject* myProject);
    bool pragaShell(PragaProject* myProject);


#endif // PRAGASHELL_H
