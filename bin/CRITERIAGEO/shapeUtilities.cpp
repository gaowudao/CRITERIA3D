#include "shapeUtilities.h"
#include <QFile>
#include <QFileInfo>


// make a copy of shapefile and return cloned shape file complete path
QString cloneShapeFile(std::string completeShapePath, QString fileName)
{
    QFileInfo filepathInfo(QString::fromStdString(completeShapePath));
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

    QString completeClonedShapePath = path + "/" + fileName + ".shp";

    return completeClonedShapePath;
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
