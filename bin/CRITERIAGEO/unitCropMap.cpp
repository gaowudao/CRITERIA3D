#include "unitCropMap.h"
#include "zonalStatistic.h"
#include "shapeToRaster.h"
#include <QFile>
#include <QFileInfo>

// 1) copiare crop > UCM (memoria se si può fare)
// 2) zonalstatistic UCM MeteoGrid (id_meteo)
// 3) zonalstatistic UCM Soil (id_soil)
// 4) add UCM agli shape in interfaccia

bool unitCropMap(Crit3DShapeHandler *ucm, Crit3DShapeHandler *crop, Crit3DShapeHandler *soil,
                 Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, double cellSize,
                 QString fileName, std::string *error)
{
    // make a copy of crop shapefile
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
        *error = "Load shapefile failed: " + ucmShapeFile.toStdString();
        return false;
    }

    gis::Crit3DRasterGrid* rasterRef = new(gis::Crit3DRasterGrid);
    gis::Crit3DRasterGrid* rasterVal = new(gis::Crit3DRasterGrid);

    // reference and value raster
    initializeRasterFromShape(ucm, rasterRef, cellSize);
    initializeRasterFromShape(ucm, rasterVal, cellSize);

    // ECM --> reference
    fillRasterWithShapeNumber(rasterRef, ucm, true);

    // meteo grid
    fillRasterWithShapeNumber(rasterVal, meteo, true);
    bool isOk = zonalStatisticsShape(ucm, meteo, rasterRef, rasterVal, idMeteo, MAJORITY, error);
    if (isOk)
    {
        // soil map
        fillRasterWithShapeNumber(rasterVal, soil, true);
        isOk = zonalStatisticsShape(ucm, soil, rasterRef, rasterVal, idSoil, MAJORITY, error);
    }

    if (! isOk)
    {
        *error = "ZonalStatisticsShape: " + *error;
    }
    delete rasterRef;
    delete rasterVal;

    return isOk;
}
