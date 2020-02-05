#include "unitCropMap.h"
#include "zonalStatistic.h"
#include "shapeToRaster.h"
#include "shapeUtilities.h"
#include <QFile>
#include <QFileInfo>

// 1) copiare crop > UCM (memoria se si può fare)
// 2) zonalstatistic UCM MeteoGrid (id_meteo)
// 3) zonalstatistic UCM Soil (id_soil)
// 4) add UCM agli shape in interfaccia

bool unitCropMap(Crit3DShapeHandler *ucm, Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo,
                 std::string idCrop, std::string idSoil, std::string idMeteo, double cellSize,
                 QString ucmFileName, std::string *error, bool showInfo)
{

    // make a copy of shapefile and return cloned shapefile complete path
    QString refShapeFilePath = QString::fromStdString(crop->getFilepath());
    QString ucmShapeFile = cloneShapeFile(refShapeFilePath, ucmFileName);
    if (!ucm->open(ucmShapeFile.toStdString()))
    {
        *error = "Load shapefile failed: " + ucmShapeFile.toStdString();
        return false;
    }

    // create reference and value raster
    gis::Crit3DRasterGrid* rasterRef = new(gis::Crit3DRasterGrid);
    gis::Crit3DRasterGrid* rasterVal = new(gis::Crit3DRasterGrid);
    initializeRasterFromShape(ucm, rasterRef, cellSize);
    initializeRasterFromShape(ucm, rasterVal, cellSize);

    // ECM --> reference
    fillRasterWithShapeNumber(rasterRef, ucm, showInfo);

    // meteo grid
    fillRasterWithShapeNumber(rasterVal, meteo, showInfo);
    bool isOk = zonalStatisticsShape(ucm, meteo, rasterRef, rasterVal, idMeteo, "ID_METEO", MAJORITY, error, showInfo);
    if (isOk)
    {
        // soil map
        fillRasterWithShapeNumber(rasterVal, soil, showInfo);
        isOk = zonalStatisticsShape(ucm, soil, rasterRef, rasterVal, idSoil, "ID_SOIL", MAJORITY, error, showInfo);
    }

    if (! isOk)
    {
        *error = "ZonalStatisticsShape: " + *error;
    }
    delete rasterRef;
    delete rasterVal;

    // add ID CASE e ID CROP
    int nShape = ucm->getShapeCount();
    int cropIndex = ucm->getFieldPos(idCrop);
    int soilIndex = ucm->getFieldPos(idSoil);
    int meteoIndex = ucm->getFieldPos(idMeteo);

    ucm->addField("ID_CROP", FTString, 5, 0);
    ucm->addField("ID_CASE", FTString, 20, 0);
    int idCaseIndex = ucm->getFieldPos("ID_CASE");
    int idCropIndex = ucm->getFieldPos("ID_CROP");

    // FILL ID_CROP and ID_CASE
    for (int shapeIndex = 0; shapeIndex < nShape; shapeIndex++)
    {
        std::string cropStr = ucm->readStringAttribute(shapeIndex, cropIndex);
        if (cropStr == "-9999") cropStr = "";

        std::string soilStr = ucm->readStringAttribute(shapeIndex, soilIndex);
        if (soilStr == "-9999") soilStr = "";

        std::string meteoStr = ucm->readStringAttribute(shapeIndex, meteoIndex);
        if (meteoStr == "-9999") meteoStr = "";

        std::string caseStr = "";
        if (meteoStr != "" && soilStr != "" && cropStr != "")
            caseStr = "M" + meteoStr + "S" + soilStr + "C" + cropStr;

        ucm->writeStringAttribute(shapeIndex, idCropIndex, cropStr.c_str());
        ucm->writeStringAttribute(shapeIndex, idCaseIndex, caseStr.c_str());
    }

    return isOk;
}
