#ifndef DATAHANDLER
#define DATAHANDLER

class QVariant;
class QDate;
class Crit3DDate;
class QString;

bool getValue(QVariant myRs, int* myValue);
bool getValue(QVariant myRs, float* myValue);
bool getValue(QVariant myRs, double* myValue);
bool getValue(QVariant myRs, QString* myValue);

Crit3DDate getCrit3DDate(const QDate &myDate);


#endif // DATAHANDLER

