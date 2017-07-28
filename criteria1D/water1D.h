#ifndef WATER1D_H
#define WATER1D_H

    class Criteria1D;
    class QString;

    void initializeWater(Criteria1D* myCase);
    bool infiltration(Criteria1D* myCase, QString* myError, float prec, float sprayIrrigation);
    bool computeRunoff(Criteria1D* myCase);
    bool surfaceRunoff(Criteria1D* myCase);
    bool subSurfaceRunoff(Criteria1D* myCase);

#endif // WATER

