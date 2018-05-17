#ifndef WATER1D_H
#define WATER1D_H

    class Criteria1D;

    void initializeWater(Criteria1D* myCase);
    bool computeInfiltration(Criteria1D* myCase, float prec, float sprayIrrigation);
    bool computeSurfaceRunoff(Criteria1D* myCase);
    bool computeLateralDrainage(Criteria1D* myCase);
    bool computeCapillaryRise(Criteria1D* myCase, float waterTableDepth);

#endif // WATER1D_H

