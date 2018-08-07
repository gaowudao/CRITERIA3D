#ifndef QUALITY_H
#define QUALITY_H

    #ifndef METEO_H
        #include "meteo.h"
    #endif

    class Crit3DMeteoPoint;

    namespace quality
    {
        enum qualityType {missing_data, wrong_syntactic, wrong_spatial, wrong_variable, accepted};

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
        quality::Range* qualityTransmissivity;

        quality::Range* qualityDailyT;
        quality::Range* qualityDailyP;
        quality::Range* qualityDailyRH;
        quality::Range* qualityDailyWInt;
        quality::Range* qualityDailyWDir;
        quality::Range* qualityDailyGRad;
        quality::Range* qualityDailyWaterTable;

    public:

        Crit3DQuality();

        quality::Range* getQualityRange(meteoVariable myVar);

        void syntacticQualityControl(meteoVariable myVar, Crit3DMeteoPoint* meteoPoints, int nrMeteoPoints);

        quality::qualityType syntacticQualitySingleValue(meteoVariable myVar, float myValue);
};


#endif // QUALITY_H
