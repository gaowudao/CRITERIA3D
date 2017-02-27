#ifndef WATER1D
#define WATER1D

class Criteria1D;
class QString;


void initializeWater(Criteria1D* myCase);
bool empiricalInfiltration(Criteria1D* myCase, QString* myError, float prec, float sprayIrrigation);
bool computeRunoff(Criteria1D* myCase);
bool surfaceRunoff(Criteria1D* myCase);
bool subSurfaceRunoff(Criteria1D* myCase);

#endif // WATER

