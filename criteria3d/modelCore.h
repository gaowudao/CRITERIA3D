#ifndef MODELCORE_H
#define MODELCORE_H

    class Crit3DProject;
    class Crit3DTime;

    bool computeET0Map(Crit3DProject* myProject);

    bool runModel(bool isInitialState, Crit3DTime myCurrentTime, Crit3DProject* myProject);

#endif // MODELCORE_H
