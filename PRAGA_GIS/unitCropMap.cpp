#include "unitCropMap.h"
#include "zonalStatistic.h"
#include <QFile>
#include <QFileInfo>

// function unitCropMap(6 campi)
// 1) copiare crop > UCM (memoria se si può fare)
// 2) zonalstatistic UCM MeteoGrid (id_meteo)
// 3) zonalstatistic UCM Soil (id_soil)
// 4) add UCM agli shape in interfaccia

bool unitCropMap(Crit3DShapeHandler *ucm, Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, int cellSize, QString fileName, std::string *error)
{

    // make a copy
    QFileInfo filepathInfo(QString::fromStdString(crop->getFilepath()));
    QString path = filepathInfo.absolutePath();
    QString ucmFile = filepathInfo.absolutePath()+"/"+fileName+".dbf";

    QFile::copy(path+"/"+filepathInfo.baseName()+".dbf", ucmFile);
    ucmFile = path+"/"+fileName+".shp";
    QFile::copy(path+"/"+filepathInfo.baseName()+".shp", ucmFile);
    ucmFile = path+"/"+fileName+".shx";
    QFile::copy(path+"/"+filepathInfo.baseName()+".shx", ucmFile);
    ucmFile = path+"/"+fileName+".prj";
    QFile::copy(path+"/"+filepathInfo.baseName()+".prj", ucmFile);

    QString fileNameWithPath = filepathInfo.absolutePath()+"/"+fileName+".shp";

    if (!ucm->open(fileNameWithPath.toStdString()))
    {
        qDebug("Load shapefile failed!");
        return false;
    }

    bool ucmSoil = zonalStatisticsShape(ucm, soil, idSoil, cellSize, MAJORITY, error);
    if (!ucmSoil)
    {
        *error = "zonalStatisticsShape ucm soil Error";
        return false;
    }

    bool ucmMeteo = zonalStatisticsShape(ucm, meteo, idMeteo, cellSize, MAJORITY, error);
    if (!ucmMeteo)
    {
        *error = "zonalStatisticsShape ucm meteo Error";
        return false;
    }
    return true;


}
