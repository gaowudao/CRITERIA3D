#ifndef WATER1D_H
#define WATER1D_H

    #include <string>
    class Criteria1D;

    void initializeWater(Criteria1D* myCase);
    bool computeInfiltration(Criteria1D* myCase, std::string* myError, float prec, float sprayIrrigation);
    bool computeSurfaceRunoff(Criteria1D* myCase);
    bool computeLateralDrainage(Criteria1D* myCase);
    bool computeCapillaryRise(Criteria1D* myCase, float waterTableDepth);

#endif // WATER

