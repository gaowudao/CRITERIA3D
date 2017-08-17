#ifndef QUALITY_H
#define QUALITY_H

    #ifndef METEOPOINT_H
        #include "meteoPoint.h"
    #endif

    namespace quality
    {
        class Range {
            private:
                float max;
                float min;
            public:
                Range();
                Range(float myMin, float myMax);

                float getMin();
                float getMax();
        };
    }

    class Crit3DQuality {

    private:
        quality::Range* qualityHourlyT;
        quality::Range* qualityHourlyTd;
        quality::Range* qualityHourlyP;
        quality::Range* qualityHourlyRH;
        quality::Range* qualityHourlyWInt;
        quality::Range* qualityHourlyWDir;
        quality::Range* qualityHourlyGIrr;

        quality::Range* qualityDailyT;
        quality::Range* qualityDailyP;
        quality::Range* qualityDailyRH;
        quality::Range* qualityDailyWInt;
        quality::Range* qualityDailyWDir;
        quality::Range* qualityDailyGRad;
        quality::Range* qualityDailyWaterTable;

        bool spatialControlActive;

    public:

        void setSpatialControl(bool isActive);

        Crit3DQuality();

        quality::Range* getQualityRange(meteoVariable myVar);

        void syntacticQualityControl(meteoVariable myVar, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints);

        bool checkData(meteoVariable myVar, frequencyType myFrequency,
                      Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints, Crit3DTime myTime);
    };

    bool passDataToInterpolation(Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints);

    void spatialQualityControl(meteoVariable myVar, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints);


#endif // QUALITY_H
