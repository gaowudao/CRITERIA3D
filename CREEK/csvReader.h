#ifndef CSVREADER_H
#define CSVREADER_H

#include <QString>
#include <vector>

bool readCSV(QString fileName, QString separator, int nrHeaderLines, QString *errorString, std::vector<QStringList> *values);


#endif // CSVREADER_H
