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
    if (!ucm->open(ucmShapeFile.toStdString()))
    {
        qDebug("Load shapefile failed!");
        return false;
    }

    gis::Crit3DRasterGrid* rasterRef = new(gis::Crit3DRasterGrid);
    gis::Crit3DRasterGrid* rasterVal = new(gis::Crit3DRasterGrid);

    // reference and value raster
    initializeRasterFromShape(ucm, rasterRef, cellSize);
    initializeRasterFromShape(ucm, rasterVal, cellSize);

    // ECM --> reference
    fillRasterWithShapeNumber(rasterRef, ucm, true);

    // soil map
    /* to do FIX
    fillRasterWithShapeNumber(rasterVal, soil, true);

    bool ucmSoil = zonalStatisticsShape(ucm, soil, rasterRef, rasterVal, idSoil, MAJORITY, error);
    if (!ucmSoil)
    {
        *error = "zonalStatisticsShape ucm soil Error";
        delete rasterRef;
        delete rasterVal;
        return false;
    }*/

    // meteo grid
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
