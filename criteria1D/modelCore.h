#ifndef MODELCORE_H
#define MODELCORE_H

    #ifndef _STRING_
        #include <string>
    #endif

    class Crit3DDate;
    class Criteria1D;
    class Criteria1DUnit;

    bool runModel(Criteria1D* myCase, Criteria1DUnit *myUnit, std::string* myError);
    bool computeModel(Criteria1D* myCase, const Crit3DDate& firstDate, const Crit3DDate& lastDate, std::string* myError);

#endif // MODELCORE_H
