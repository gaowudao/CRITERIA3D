#ifndef MODELCORE_H
#define MODELCORE_H


    class Crit3DDate;
    class Criteria1D;
    class Criteria1DUnit;
    class QString;

    bool runModel(Criteria1D* myCase, Criteria1DUnit *myUnit, QString* myError);
    bool computeModel(Criteria1D* myCase, const Crit3DDate& firstDate, const Crit3DDate& lastDate, QString* myError);

#endif // MODELCORE_H
