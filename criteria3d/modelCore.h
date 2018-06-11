#ifndef MODELCORE_H
#define MODELCORE_H

    #ifndef QSTRING_H
        #include <QString>
    #endif
    #ifndef CRIT3DDATE_H
        #include "crit3dDate.h"
    #endif

    class Crit3DProject;

    bool modelDailyCycle(bool isInitialState, Crit3DDate myDate, int nrHours, Crit3DProject* myProject,
                         const QString& myOutputPath, bool isSave, const QString& myArea);

#endif // MODELCORE_H
