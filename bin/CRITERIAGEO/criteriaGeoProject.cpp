/*!
    \file gisProject.cpp

    \abstract Praga-Gis project class

    This file is part of CRITERIA-3D distribution.

    CRITERIA-3D has been developed by A.R.P.A.E. Emilia-Romagna.

    \copyright
    CRITERIA-3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    CRITERIA-3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA-3D.  If not, see <http://www.gnu.org/licenses/>.

    \authors
    Fausto Tomei ftomei@arpae.it
    Gabriele Antolini gantolini@arpae.it
    Laura Costantini laura.costantini0@gmail.com
*/

#include "commonConstants.h"
#include "criteriaGeoProject.h"
#include "shapeToRaster.h"
#include "unitCropMap.h"
#include "ucmUtilities.h"

#include <QMessageBox>
#include <QFile>


CriteriaGeoProject::CriteriaGeoProject()
{}


void CriteriaGeoProject::addRaster(gis::Crit3DRasterGrid *myRaster, QString fileName)
{
    GisObject* newObject = new(GisObject);
    newObject->setRaster(fileName, myRaster);
    this->objectList.push_back(newObject);
}


void CriteriaGeoProject::addShapeFile(Crit3DShapeHandler *myShape, QString fileName)
{
    GisObject* newObject = new(GisObject);
    newObject->setShapeFile(fileName, myShape);
    this->objectList.push_back(newObject);
}


bool CriteriaGeoProject::loadRaster(QString fileNameWithPath)
{
    std::string* myError = new std::string();
    std::string fnWithoutExt = fileNameWithPath.left(fileNameWithPath.length()-4).toStdString();

    gis::Crit3DRasterGrid *myRaster = new(gis::Crit3DRasterGrid);
    if (! gis::readEsriGrid(fnWithoutExt, myRaster, myError))
    {
        qDebug("Load raster failed!");
        return false;
    }

    setDefaultDEMScale(myRaster->colorScale);
    addRaster(myRaster, fileNameWithPath);
    return true;
}


bool CriteriaGeoProject::loadShapefile(QString fileNameWithPath)
{
    Crit3DShapeHandler *myShape = new(Crit3DShapeHandler);
    if (!myShape->open(fileNameWithPath.toStdString()))
    {
        qDebug("Load shapefile failed!");
        return false;
    }

    addShapeFile(myShape, fileNameWithPath);
    return true;
}


void CriteriaGeoProject::getRasterFromShape(Crit3DShapeHandler *shape, QString field, QString outputName, double cellSize, bool showInfo)
{
    gis::Crit3DRasterGrid *newRaster = new(gis::Crit3DRasterGrid);
    initializeRasterFromShape(shape, newRaster, cellSize);

    if (field == "Shape ID")
    {
        fillRasterWithShapeNumber(newRaster, shape, showInfo);
    }
    else
    {
        fillRasterWithField(newRaster, shape, field.toStdString(), showInfo);
    }

    gis::updateMinMaxRasterGrid(newRaster);
    setTemperatureScale(newRaster->colorScale);
    addRaster(newRaster, outputName);
}


bool CriteriaGeoProject::addUnitCropMap(Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo,
                                std::string idCrop, std::string idSoil, std::string idMeteo,
                                QString fileName, double cellSize, bool showInfo)
{
    std::string errorStr;

    Crit3DShapeHandler *ucm = new(Crit3DShapeHandler);

    if (unitCropMap(ucm, crop, soil, meteo, idCrop, idSoil, idMeteo, cellSize, fileName, &errorStr, showInfo))
    {
        addShapeFile(ucm, QString::fromStdString(ucm->getFilepath()));
        return true;
    }
    else
    {
        logError(errorStr);
        return false;
    }
}

bool CriteriaGeoProject::extractUCMListToDb(int pos, QString dbName, bool showInfo)
{
    Crit3DShapeHandler* shapeHandler = (objectList.at(unsigned(pos)))->getShapeHandler();
    std::string errorStr;

    int fieldRequired = 0;
    for (int i = 0; i < shapeHandler->getFieldNumbers(); i++)
    {

        if (shapeHandler->getFieldName(i) == "ID_CASE" || shapeHandler->getFieldName(i) == "ID_SOIL" || shapeHandler->getFieldName(i) == "ID_CROP" || shapeHandler->getFieldName(i) == "ID_METEO")
        {
            fieldRequired = fieldRequired + 1;
        }
    }
    if (fieldRequired < 4)
    {
        errorStr = "Ivalid Unit Crop Map - Missing required fields";
        logError(errorStr);
        return false;
    }

    QFile dbFile(dbName);
    if (dbFile.exists())
    {
        if (!dbFile.remove())
        {
            QString error = "Remove file failed: " + dbName + "\n" + dbFile.errorString();
            logError(error.toStdString());
            return false;
        }
    }

    if (UCMListToDb(shapeHandler, dbName, &errorStr, showInfo))
    {
        return true;
    }
    else
    {
        logError(errorStr);
        return false;
    }
}

bool CriteriaGeoProject::createShapeFromCSV(int pos, QString fileCSV, QString fileCSVRef)
{
    Crit3DShapeHandler* shapeHandler = (objectList.at(unsigned(pos)))->getShapeHandler();
    std::string errorStr;

    bool found = false;
    for (int i = 0; i < shapeHandler->getFieldNumbers(); i++)
    {
        if (shapeHandler->getFieldName(i) == "ID_CASE")
        {
            found = true;
        }
    }
    if (!found)
    {
        errorStr = "Ivalid Unit Crop Map - Missing ID_CASE";
        logError(errorStr);
        return false;
    }

    Crit3DShapeHandler *outputShape = new Crit3DShapeHandler;
    if (shapeFromCSV(shapeHandler, outputShape, fileCSV, fileCSVRef, &errorStr))
    {
        return true;
    }
    else
    {
        logError(errorStr);
        return false;
    }
}


//--------------------------------------------------------------
// LOG
//--------------------------------------------------------------

void CriteriaGeoProject::logError(std::string errorString)
{
    QMessageBox::critical(nullptr, "ERROR!", QString::fromStdString(errorString));
}

void CriteriaGeoProject::logError(QString errorString)
{
    QMessageBox::critical(nullptr, "ERROR!", errorString);
}

