#ifndef INTERPOLATIONPOINT_H
#define INTERPOLATIONPOINT_H

    #ifndef GIS_H
        #include "gis.h"
    #endif

    class Crit3DInterpolationDataPoint {
    private:


    public:
        gis::Crit3DPoint* point;
        int index;
        bool isActive;
        float distance;
        float deltaZ;
        float value;
        float detrendValue;
        int lapseRateCode;
        int indexTopoDistMap;
        std::vector <float> proxyValues;

        float getProxyValue(unsigned int pos);
        std::vector <float> Crit3DInterpolationDataPoint::getProxyValues();

        Crit3DInterpolationDataPoint();
    };

#endif // INTERPOLATIONPOINT_H
