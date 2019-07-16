#ifndef PRAGASHELL_H
#define PRAGASHELL_H

    #ifndef SHELL_H
        #include "shell.h"
    #endif

    #ifndef PRAGAPROJECT_H
        #include "pragaProject.h"
    #endif

    bool executePragaCommand(std::vector<std::string> command, PragaProject* myProject);
    bool pragaShell(PragaProject* myProject);


#endif // PRAGASHELL_H
