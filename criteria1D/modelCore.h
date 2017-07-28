#ifndef MODELCORE_H
#define MODELCORE_H

    #include <string>
    class Crit3DDate;
    class Criteria1D;

    bool runModel(Criteria1D* myCase, std::string* myError, Criteria1DUnit *myUnit);
    bool computeModel(Criteria1D* myCase, std::string* myError, const Crit3DDate& firstDate, const Crit3DDate& lastDate);

#endif // MODELCORE_H
