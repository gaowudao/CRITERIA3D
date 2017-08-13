#ifndef TRANSMISSIVITY_H
#define TRANSMISSIVITY_H

    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif

    float computePointTransmissivitySamani(float tmin, float tmax, float samaniCoeff);

    float computePointTransmissivity(const gis::Crit3DPoint& myPoint, Crit3DTime UTCTime, float* measuredRad,
                                 int windowWidth, int timeStepSecond, const gis::Crit3DRasterGrid& myDtm);

    int computeTransmissivity(Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints, int intervalWidth,
                                Crit3DTime myTime, const gis::Crit3DRasterGrid& myDtm);

    int computeTransmissivityFromTRange(Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints, Crit3DTime currentTime);

#endif // TRANSMISSIVITY_H
