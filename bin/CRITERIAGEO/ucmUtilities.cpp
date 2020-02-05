#include "ucmUtilities.h"
#include "shapeUtilities.h"
#include "ucmDb.h"
#include "commonConstants.h"
#include "formInfo.h"


bool UCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error, bool showInfo)
{
    UcmDb* unitList = new UcmDb(dbName);

    QStringList idCase;
    QStringList idCrop;
    QStringList idMeteo;
    QList<float> idSoil;

    int nShape = shapeHandler->getShapeCount();

    FormInfo formInfo;
    if (showInfo)
    {
        formInfo.start("Extract list " + QString::fromStdString(shapeHandler->getFilepath()), nShape);
    }

    for (int i = 0; i < nShape; i++)
    {
        if (showInfo)
        {
            formInfo.setValue(i);
        }
        QString key = QString::fromStdString(shapeHandler->getStringValue(signed(i), "ID_CASE"));
        if (!key.isEmpty() && !idCase.contains(key))
        {
            idCase << key;
            idCrop << QString::fromStdString(shapeHandler->getStringValue(signed(i), "ID_CROP"));
            idMeteo << QString::fromStdString(shapeHandler->getStringValue(signed(i), "ID_METEO"));
            idSoil << shapeHandler->getNumericValue(signed(i), "ID_SOIL");
        }
    }
    bool res = unitList->writeListToUnitsTable(idCase, idCrop, idMeteo, idSoil);
    *error = unitList->getError().toStdString();

    delete unitList;

    if (showInfo)
    {
        formInfo.close();
    }

    return res;
}

/*
bool shapeFromCSV(Crit3DShapeHandler* shapeHandler, Crit3DShapeHandler* outputShape, QString fileCSV, QString fileCSVRef, std::string *error)
{

    QFileInfo fileCSVpathInfo(fileCSV);
    QString fileName = fileCSVpathInfo.baseName();

// make a copy of shapefile and return cloned shapefile complete path
    QString ucmShapeFile = cloneShapeFile(shapeHandler->getFilepath(), fileName);

    if (!outputShape->open(ucmShapeFile.toStdString()))
    {
        *error = "Load shapefile failed: " + ucmShapeFile.toStdString();
        return false;
    }
    int nShape = outputShape->getShapeCount();

    QFile file(fileCSV);
    if ( !file.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else
    {
        // Create a thread to retrieve data from a file
        QTextStream in(&file);
        // Reads first row
        QString firstRow = in.readLine();
        QStringList newFields = firstRow.split(",");

        int type;
        int nWidth;
        int nDecimals;

        QMap<int, int> myPosMap;

        int idCaseIndex = outputShape->getFieldPos("ID_CASE");
        int idCaseCSV = NODATA;

        for (int i = 0; i < newFields.size(); i++)
        {
            if (newFields[i] == "ID_CASE")
            {
                idCaseCSV = i;
            }
            if (MapCSVShapeFields.contains(newFields[i]))
            {
                if (newFields[i] == "CROP")
                {
                    type = FTString;
                    nWidth = 20;
                    nDecimals = 0;
                }
                else
                {
                    type = FTDouble;
                    nWidth = 10;
                    nDecimals = 2;
                }
                outputShape->addField(MapCSVShapeFields.value(newFields[i]).toStdString().c_str(), type, nWidth, nDecimals);
                myPosMap.insert(i,outputShape->getFieldPos(MapCSVShapeFields.value(newFields[i]).toStdString()));
            }

        }

        if (idCaseCSV == NODATA)
        {
            *error = "invalid CSV, missing ID_CASE";
            return false;
        }

        //Reads the data up to the end of file
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList items = line.split(",");
            //qDebug() << "items " << items;
            for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
            {
                // check right ID_CASE
                if (outputShape->readStringAttribute(shapeIndex, idCaseIndex) == items[idCaseCSV].toStdString())
                {
                    QMapIterator<int, int> i(myPosMap);
                    while (i.hasNext()) {
                        i.next();
                        QString valueToWrite = items[i.key()];
                        if (outputShape->getFieldType(i.value()) == FTString)
                        {
                            outputShape->writeStringAttribute(shapeIndex, i.value(), valueToWrite.toStdString().c_str());
                        }
                        else
                        {
                            outputShape->writeDoubleAttribute(shapeIndex, i.value(), valueToWrite.toDouble());
                        }

                    }

                }
            }
        }
        file.close();
    }

    return true;
}
*/
bool shapeFromCSV(Crit3DShapeHandler* shapeHandler, Crit3DShapeHandler* outputShape, QString fileCSV, QString fileCSVRef, std::string *error)
{

    int CSVRefRequiredInfo = 5;
    int defaultStringLenght = 20;
    int defaultDoubleLenght = 10;
    int defaultDoubleDecimals = 2;

    QFileInfo fileCSVpathInfo(fileCSV);
    QString fileName = fileCSVpathInfo.baseName();

    // make a copy of shapefile and return cloned shapefile complete path
    QString ucmShapeFile = cloneShapeFile(shapeHandler->getFilepath(), fileName);

    if (!outputShape->open(ucmShapeFile.toStdString()))
    {
        *error = "Load shapefile failed: " + ucmShapeFile.toStdString();
        return false;
    }

    // read fileCSVRef and fill MapCSVShapeFields
    QMap<QString, QStringList> MapCSVShapeFields;
    QFile fileRef(fileCSVRef);
    if ( !fileRef.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else
    {
        QTextStream in(&fileRef);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList items = line.split(",");
            if (items.size() < CSVRefRequiredInfo)
            {
                *error = "invalid reference CSV, missing reference data";
                return false;
            }
            QString key = items[0];
            items.removeFirst();
            if (key.isEmpty() || items[0].isEmpty())
            {
                *error = "invalid reference CSV, missing field name";
                return false;
            }
            MapCSVShapeFields.insert(key,items);
        }
    }


    int nShape = outputShape->getShapeCount();

    QFile file(fileCSV);
    if ( !file.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else
    {
        // Create a thread to retrieve data from a file
        QTextStream in(&file);
        // Reads first row
        QString firstRow = in.readLine();
        QStringList newFields = firstRow.split(",");

        int type;
        int nWidth;
        int nDecimals;

        QMap<int, int> myPosMap;

        int idCaseIndex = outputShape->getFieldPos("ID_CASE");
        int idCaseCSV = NODATA;

        for (int i = 0; i < newFields.size(); i++)
        {
            if (newFields[i] == "ID_CASE")
            {
                idCaseCSV = i;
            }
            if (MapCSVShapeFields.contains(newFields[i]))
            {
                QStringList valuesList = MapCSVShapeFields.value(newFields[i]);
                QString field = valuesList[0];
                if (valuesList[1] == "STRING")
                {
                    type = FTString;
                    if (valuesList[2].isEmpty())
                    {
                        nWidth = defaultStringLenght;
                    }
                    else
                    {
                        nWidth = valuesList[2].toInt();
                    }
                    nDecimals = 0;
                }
                else
                {
                    type = FTDouble;
                    if (valuesList[2].isEmpty())
                    {
                        nWidth = defaultDoubleLenght;
                    }
                    else
                    {
                        nWidth = valuesList[2].toInt();
                    }
                    if (valuesList[3].isEmpty())
                    {
                        nDecimals = defaultDoubleDecimals;
                    }
                    else
                    {
                        nDecimals = valuesList[3].toInt();
                    }
                }
                outputShape->addField(field.toStdString().c_str(), type, nWidth, nDecimals);
                myPosMap.insert(i,outputShape->getFieldPos(field.toStdString()));
            }

        }

        if (idCaseCSV == NODATA)
        {
            *error = "invalid CSV, missing ID_CASE";
            return false;
        }

        //Reads the data up to the end of file
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList items = line.split(",");
            //qDebug() << "items " << items;
            for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
            {
                // check right ID_CASE
                if (outputShape->readStringAttribute(shapeIndex, idCaseIndex) == items[idCaseCSV].toStdString())
                {
                    QMapIterator<int, int> i(myPosMap);
                    while (i.hasNext()) {
                        i.next();
                        QString valueToWrite = items[i.key()];
                        if (outputShape->getFieldType(i.value()) == FTString)
                        {
                            outputShape->writeStringAttribute(shapeIndex, i.value(), valueToWrite.toStdString().c_str());
                        }
                        else
                        {
                            outputShape->writeDoubleAttribute(shapeIndex, i.value(), valueToWrite.toDouble());
                        }

                    }

                }
            }
        }
        file.close();
    }

    return true;
}
