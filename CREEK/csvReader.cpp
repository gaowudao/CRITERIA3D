#include <QFile>
#include <QTextStream>
#include <iostream>
#include "csvReader.h"

using namespace std;

bool readCSV(QString fileName, QString separator, int nrHeaderLines, QString *errorString, std::vector<QStringList> *values)
{
    QFile file(fileName);
    if(! file.open (QIODevice::ReadOnly))
    {
        *errorString = file.errorString() + "\n" + fileName;
        return false;
    }

    QTextStream stream (&file);
    QStringList line;

    int nrLine = 0;
    while(!stream.atEnd())
    {
        line = stream.readLine().split(separator);

        // skip header and void lines
        if ((nrLine >= nrHeaderLines) && (line.length()>1))
            values->push_back(line);

        nrLine++;
    }

    file.close ();

    return true;
}

