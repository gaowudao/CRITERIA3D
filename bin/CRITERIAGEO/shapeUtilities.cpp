#include "shapeUtilities.h"
#include "unitDb.h"
#include "commonConstants.h"


bool extractUCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error)
{
    UnitDb* unitList = new UnitDb(dbName);

    QStringList idCase;
    QStringList idCrop;
    QStringList idMeteo;
    QList<float> idSoil;

    int nShape = shapeHandler->getShapeCount();

    for (int i = 0; i < nShape; i++)
    {
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

    return res;
}


bool createShapeFromCSV(Crit3DShapeHandler* shapeHandler, Crit3DShapeHandler* shapeFromCSV, QString fileCSV, std::string *error)
{

    QFileInfo fileCSVpathInfo(fileCSV);
    QString fileName = fileCSVpathInfo.baseName();

    // make a copy of shapefile (copia tutto, oppure deve copiare solo ID_CASE?)
    QFileInfo filepathInfo(QString::fromStdString(shapeHandler->getFilepath()));
    QString path = filepathInfo.absolutePath();

    QString tmpFile = filepathInfo.absolutePath()+"/"+fileName+".dbf";
    QFile::remove(tmpFile);
    QFile::copy(path+"/"+filepathInfo.baseName()+".dbf", tmpFile);

    tmpFile = path+"/"+fileName+".shp";
    QFile::remove(tmpFile);
    QFile::copy(path+"/"+filepathInfo.baseName()+".shp", tmpFile);

    tmpFile = path+"/"+fileName+".shx";
    QFile::remove(tmpFile);
    QFile::copy(path+"/"+filepathInfo.baseName()+".shx", tmpFile);

    tmpFile = path+"/"+fileName+".prj";
    QFile::remove(tmpFile);
    QFile::copy(path+"/"+filepathInfo.baseName()+".prj", tmpFile);

    QString ucmShapeFile = path + "/" + fileName + ".shp";
    if (!shapeFromCSV->open(ucmShapeFile.toStdString()))
    {
        *error = "Load shapefile failed: " + ucmShapeFile.toStdString();
        return false;
    }
    int nShape = shapeFromCSV->getShapeCount();

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

        int idCaseIndex = shapeFromCSV->getFieldPos("ID_CASE");
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
                shapeFromCSV->addField(MapCSVShapeFields.value(newFields[i]).toStdString().c_str(), type, nWidth, nDecimals);
                myPosMap.insert(i,shapeFromCSV->getFieldPos(MapCSVShapeFields.value(newFields[i]).toStdString()));
            }

        }

        if (idCaseCSV == NODATA)
        {
            *error = "invalid CSV, missing ID_CASE";
            return false;
        }

        bool idFound = false;
        //Reads the data up to the end of file
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList items = line.split(",");
            //qDebug() << "items " << items;
            for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
            {
                // check right ID_CASE
                if (shapeFromCSV->readStringAttribute(shapeIndex, idCaseIndex) == items[idCaseCSV].toStdString())
                {
                    idFound = true;
                    QMapIterator<int, int> i(myPosMap);
                    while (i.hasNext()) {
                        i.next();
                        QString valueToWrite = items[i.key()];
                        if (shapeFromCSV->getFieldType(i.value()) == FTString)
                        {
                            shapeFromCSV->writeStringAttribute(shapeIndex, i.value(), valueToWrite.toStdString().c_str());
                        }
                        else
                        {
                            shapeFromCSV->writeDoubleAttribute(shapeIndex, i.value(), valueToWrite.toDouble());
                        }

                    }

                }
                if (idFound == true)
                {
                    idFound = false;
                    break;
                }
            }
        }
        file.close();
    }

    return true;
}
