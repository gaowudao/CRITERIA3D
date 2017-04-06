#ifndef FILEUTILITY
#define FILEUTILITY

#ifndef QSTRING_H
    #include <QString>
#endif

struct ToutputDailyMeteo;
struct TinputObsData;

bool readCsv (QString namefile, char separator, float noData,  TinputObsData* inputData);
bool writeCsv (QString namefile, char separator, ToutputDailyMeteo* mydailyData);


#endif // FILEUTILITY

