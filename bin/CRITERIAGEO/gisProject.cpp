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
#include "gisProject.h"
#include "shapeToRaster.h"
#include "unitCropMap.h"
#include <QMessageBox>


GisProject::GisProject()
{}


void GisProject::addRaster(gis::Crit3DRasterGrid *myRaster, QString fileName)
{
    GisObject* newObject = new(GisObject);
    newObject->setRaster(fileName, myRaster);
    this->objectList.push_back(newObject);
}


void GisProject::addShapeFile(Crit3DShapeHandler *myShape, QString fileName)
{
    GisObject* newObject = new(GisObject);
    newObject->setShapeFile(fileName, myShape);
    this->objectList.push_back(newObject);
}


bool GisProject::loadRaster(QString fileNameWithPath)
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


bool GisProject::loadShapefile(QString fileNameWithPath)
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


void GisProject::getRasterFromShape(Crit3DShapeHandler *shape, QString field, QString outputName, double cellSize, bool showInfo)
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


bool GisProject::addUnitCropMap(Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, QString fileName, double cellSize)
{
    std::string errorStr;

    Crit3DShapeHandler *ucm = new(Crit3DShapeHandler);

    if (unitCropMap(ucm, crop, soil, meteo, idSoil, idMeteo, cellSize, fileName, &errorStr))
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


//--------------------------------------------------------------
// LOG
//--------------------------------------------------------------

void GisProject::logError(std::string errorString)
{
    QMessageBox::critical(nullptr, "ERROR!", QString::fromStdString(errorString));
}

void GisProject::logError(QString errorString)
{
    QMessageBox::critical(nullptr, "ERROR!", errorString);
}

