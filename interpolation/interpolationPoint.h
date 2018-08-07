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
        int indexTopoDistMap;
        std::vector <float> proxyValues;

        float getProxy(unsigned int pos);

        Crit3DInterpolationDataPoint();
    };

#endif // INTERPOLATIONPOINT_H
