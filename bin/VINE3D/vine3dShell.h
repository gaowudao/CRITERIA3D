#ifndef VINE3DSHELL_H
#define VINE3DSHELL_H

#ifndef PRAGAPROJECT_H
    #include "vine3DProject.h"
#endif

bool executeVine3DCommand(QStringList argumentList, Vine3DProject* myProject);
bool executeCommand(QStringList commandLine, Vine3DProject* myProject);
bool pragaShell(Vine3DProject* myProject);
bool pragaBatch(Vine3DProject* myProject, QString batchFileName);

#endif // VINE3DSHELL_H
