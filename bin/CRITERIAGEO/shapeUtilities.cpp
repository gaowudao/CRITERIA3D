#include "shapeUtilities.h"
#include <QFile>
#include <QFileInfo>


// make a copy of shapefile and return cloned shape file complete path
QString cloneShapeFile(QString refShapeCompletePath, QString cloneFileName)
{
    QFileInfo filepathInfo(refShapeCompletePath);
    QString path = filepathInfo.absolutePath();

    QString tmpFile = filepathInfo.absolutePath()+"/"+cloneFileName+".dbf";
    QFile::remove(tmpFile);
    QFile::copy(path+"/"+filepathInfo.baseName()+".dbf", tmpFile);

    tmpFile = path+"/"+cloneFileName+".shp";
    QFile::remove(tmpFile);
    QFile::copy(path+"/"+filepathInfo.baseName()+".shp", tmpFile);

    tmpFile = path+"/"+cloneFileName+".shx";
    QFile::remove(tmpFile);
    QFile::copy(path+"/"+filepathInfo.baseName()+".shx", tmpFile);

    tmpFile = path+"/"+cloneFileName+".prj";
    QFile::remove(tmpFile);
    QFile::copy(path+"/"+filepathInfo.baseName()+".prj", tmpFile);

    QString clonedShapePath = path + "/" + cloneFileName + ".shp";
    return clonedShapePath;
}


bool deleteRecords(Crit3DShapeHandler *shapeHandler, QString newFile)
{
    if (shapeHandler->existRecordDeleted())
    {
        shapeHandler->packSHP(newFile.toStdString());
        shapeHandler->packDBF(newFile.toStdString());
        shapeHandler->close();
        return true;
    }
    return false;
}
