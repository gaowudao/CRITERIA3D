#include "unitCropMap.h"
#include "zonalStatistic.h"
#include "shapeToRaster.h"
#include <QFile>
#include <QFileInfo>

// function unitCropMap(6 campi)
// 1) copiare crop > UCM (memoria se si può fare)
// 2) zonalstatistic UCM MeteoGrid (id_meteo)
// 3) zonalstatistic UCM Soil (id_soil)
// 4) add UCM agli shape in interfaccia

bool unitCropMap(Crit3DShapeHandler *ucm, Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, double cellSize, QString fileName, std::string *error)
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

    gis::Crit3DRasterGrid* rasterRef = new(gis::Crit3DRasterGrid);
    gis::Crit3DRasterGrid* rasterVal = new(gis::Crit3DRasterGrid);

    initializeRasterFromShape(ucm, rasterRef, cellSize);
    fillRasterWithShapeNumber(rasterRef, ucm, true);

    initializeRasterFromShape(ucm, rasterVal, cellSize);
    fillRasterWithShapeNumber(rasterVal, soil, true);

    bool ucmSoil = zonalStatisticsShape(ucm, soil, rasterRef, rasterVal, idSoil, MAJORITY, error);
    if (!ucmSoil)
    {
        *error = "zonalStatisticsShape ucm soil Error";
        return false;
    }

    fillRasterWithShapeNumber(rasterVal, meteo, true);
    bool ucmMeteo = zonalStatisticsShape(ucm, meteo, rasterRef, rasterVal, idMeteo, MAJORITY, error);

    delete rasterRef;
    delete rasterVal;
    if (!ucmMeteo)
    {
        *error = "zonalStatisticsShape ucm meteo Error";
        return false;
    }
    return true;


}
