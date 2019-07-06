#ifndef FILEUTILITY_H
#define FILEUTILITY_H

    class QString;
    struct ToutputDailyMeteo;
    struct TinputObsData;

    bool readMeteoDataCsv (QString namefile, char separator, float noData,  TinputObsData* inputData);

    bool writeMeteoDataCsv (QString namefile, char separator, ToutputDailyMeteo* mydailyData);

#endif // FILEUTILITY_H

