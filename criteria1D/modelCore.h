#ifndef MODELCORE_H
#define MODELCORE_H

class QString;
class Crit3DDate;
class Criteria1D;

bool runModel(Criteria1D* myCase, QString* myError, Criteria1DUnit *myUnit);
bool computeModel(Criteria1D* myCase, QString* myError, const Crit3DDate& firstDate, const Crit3DDate& lastDate);

#endif // MODELCORE_H
