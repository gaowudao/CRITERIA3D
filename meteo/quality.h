#ifndef QUALITY_H
#define QUALITY_H

    #ifndef METEO_H
        #include "meteo.h"
    #endif

    #define QUALITY_GOOD        0
    #define QUALITY_WRONG       1
    #define QUALITY_MISSING     99

    namespace quality
    {
        enum qualityValue {missing, wrong, accepted};

        class qualityRange {
            private:
                float max;
                float min;
            public:
                qualityRange();
                qualityRange(float myMin, float myMax);

                float getMin();
                float getMax();
        };

        class qualityMeteo {

        public:
            qualityRange* qualityHourlyT;
            qualityRange* qualityHourlyTd;
            qualityRange* qualityHourlyP;
            qualityRange* qualityHourlyRH;
            qualityRange* qualityHourlyWInt;
            qualityRange* qualityHourlyWDir;
            qualityRange* qualityHourlyGIrr;

            qualityRange* qualityDailyT;
            qualityRange* qualityDailyP;
            qualityRange* qualityDailyRH;
            qualityRange* qualityDailyWInt;
            qualityRange* qualityDailyWDir;
            qualityRange* qualityDailyGRad;
            qualityRange* qualityDailyWaterTable;

            qualityMeteo();
        };

        qualityRange* getQualityRange(meteoVariable myVar, qualityMeteo myQualityMeteo);
    }

#endif // QUALITY_H
