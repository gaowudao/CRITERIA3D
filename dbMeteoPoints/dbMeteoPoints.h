#ifndef DBMETEOPOINTS_H
#define DBMETEOPOINTS_H

#include <QString>

struct TPointProperties {
    int id;
    int name;
    QString network;
    double lat;
    double lon;
    int latInt;
    int lonInt;
    double utm_x;
    double utm_y;
    float altitude;
    QString state;
    QString region;
    QString province;
    QString municipality;
};

QString getDatasetURL(QString dbName, QString dataset);
QStringList getDatasetsActive(QString dbName);
QString getVarName(QString dbName, int id);
int getId(QString dbName, QString VarName);
void fillPointProperties(QString dbName, TPointProperties* pointProp);

#endif // DBMETEOPOINTS_H
