#ifndef DBMETEOPOINTS_H
#define DBMETEOPOINTS_H

#include <QString>

QString getDatasetURL(QString dbName, QString dataset);
QStringList getDatasetsActive(QString dbName);
QString getVarName(QString dbName, int id);
int getId(QString dbName, QString VarName);

#endif // DBMETEOPOINTS_H
