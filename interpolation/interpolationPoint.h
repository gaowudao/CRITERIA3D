#ifndef INTERPOLATIONPOINT_H
#define INTERPOLATIONPOINT_H

    #ifndef GIS_H
        #include "gis.h"
    #endif

    class Crit3DInterpolationDataPoint {
    private:
        float orogIndex;
        float urbanFraction;
        float seaDistance;
        float aspect;
        float genericProxyValue;

    public:
        gis::Crit3DPoint* point;
        int index;
        bool isActive;
        float distance;
        float deltaZ;
        float value;

        void setOrogIndex(float myValue);
        void setUrbanFraction(float myValue);
        void setSeaDistance(float myValue);
        void setAspect(float myValue);
        void setGenericProxy(float myValue);

        float getOrogIndex();
        float getUrbanFraction();
        float getSeaDistance();
        float getAspect();
        float getGenericProxy();

        Crit3DInterpolationDataPoint();
    };

#endif // INTERPOLATIONPOINT_H
