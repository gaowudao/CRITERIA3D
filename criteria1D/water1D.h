#ifndef WATER1D_H
#define WATER1D_H

    #include <string>
    class Criteria1D;

    void initializeWater(Criteria1D* myCase);
    bool infiltration(Criteria1D* myCase, std::string* myError, float prec, float sprayIrrigation);
    bool computeRunoff(Criteria1D* myCase);
    bool surfaceRunoff(Criteria1D* myCase);
    bool subSurfaceRunoff(Criteria1D* myCase);
    bool capillaryRise(Criteria1D* myCase);

#endif // WATER

