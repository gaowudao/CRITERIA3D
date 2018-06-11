#ifndef MODELCORE_H
#define MODELCORE_H

    class Crit3DProject;
    class Crit3DDate;
    class QString;

    bool modelDailyCycle(bool isInitialState, Crit3DDate myDate, int nrHours, Crit3DProject* myProject,
                         const QString& myOutputPath, bool isSave, const QString& myArea);

#endif // MODELCORE_H
