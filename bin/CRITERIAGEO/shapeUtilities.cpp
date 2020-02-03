#include "shapeUtilities.h"
#include "unitDb.h"
#include "formInfo.h"

bool extractUCMListToDb(Crit3DShapeHandler* shapeHandler, QString dbName, std::string *error, bool showInfo)
{
    FormInfo formInfo;

    UnitDb* unitList = new UnitDb(dbName);

    QStringList idCase;
    QStringList idCrop;
    QStringList idMeteo;
    QList<float> idSoil;

    int nShape = shapeHandler->getShapeCount();

    formInfo.start("Extract list...", nShape);

    for (int i = 0; i < nShape; i++)
    {
        formInfo.setValue(i);

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

    formInfo.close();
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
        qDebug() << "newFields " << newFields;
        // LC caratteristiche campi, nomi, tipi?
        int type = FTString;
        int nWidth = 40;
        int nDecimals = 0;
        for (int i = 0; i < newFields.size(); i++)
        {
            qDebug() << "newFields[i] " << newFields[i];
            shapeFromCSV->addField(newFields[i].toStdString().c_str(), type, nWidth, nDecimals);
        }

        //Reads the data up to the end of file
//        while (!in.atEnd())
//        {
//            QString line = in.readLine();
//            QStringList item = line.split(";");
//            qDebug() << "item " << item;
//        }
        file.close();
    }

    return true;
}
