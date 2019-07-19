#ifndef VINE3DSHELL_H
#define VINE3DSHELL_H

#ifndef VINE3DPROJECT_H
    #include "vine3DProject.h"
#endif

bool executeCommand(QStringList commandLine, Vine3DProject* myProject);
bool vine3dShell(Vine3DProject* myProject);
bool vine3dBatch(Vine3DProject* myProject, QString batchFileName);

#endif // VINE3DSHELL_H
