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


GisObject::GisObject()
{
    this->type = gisObjectNone;
    this->fileName = "";
    this->fileNameWithPath = "";
    this->isSelected = true;
    this->rasterPtr = nullptr;
    this->shapePtr = nullptr;
}


void GisObject::setRaster(QString fileNameWithPath, gis::Crit3DRasterGrid* rasterPtr)
{
    this->type = gisObjectRaster;
    this->fileNameWithPath = fileNameWithPath;
    this->fileName = getFileName(fileNameWithPath);
    this->isSelected = true;

    this->rasterPtr = rasterPtr;
}


void GisObject::setShapeFile(QString fileNameWithPath, Crit3DShapeHandler* shapePtr)
{
    this->type = gisObjectShape;
    this->fileNameWithPath = fileNameWithPath;
    this->fileName = getFileName(fileNameWithPath);
    this->isSelected = true;

    this->shapePtr = shapePtr;
}


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
        return (false);
    }

    setDefaultDTMScale(myRaster->colorScale);
    addRaster(myRaster, fileNameWithPath);
    return (true);
}


bool GisProject::loadShapefile(QString fileNameWithPath)
{
    Crit3DShapeHandler *myShape = new(Crit3DShapeHandler);
    if (!myShape->open(fileNameWithPath.toStdString()))
    {
        qDebug("Load shapefile failed!");
        return (false);
    }

    addShapeFile(myShape, fileNameWithPath);
    return (true);
}


QString getFileName(QString fileNameWithPath)
{
    QString c;
    QString fileName = "";
    for (int i = fileNameWithPath.length()-1; i >= 0; i--)
    {
        c = fileNameWithPath.mid(i,1);
        if ((c != "\\") && (c != "/"))
            fileName = c + fileName;
        else
            return fileName;
    }
    return fileName;
}


void GisProject::getRasterFromShape(Crit3DShapeHandler *shape, QString field, QString outputName)
{
    gis::Crit3DRasterGrid *newRaster = initializeRasterFromShape(shape, CELLSIZE);
    if (field == "Shape ID")
    {
        fillRasterWithShapeNumber(newRaster, shape);
    }
    else
    {
        fillRasterWithField(newRaster, shape, field.toStdString());
    }
    gis::updateMinMaxRasterGrid(newRaster);
    addRaster(newRaster, outputName);
}

bool GisProject::addUnitCropMap(Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, QString fileName)
{
    std::string* error = new std::string();

    Crit3DShapeHandler *ucm = new(Crit3DShapeHandler);

    if (unitCropMap(ucm, crop, soil, meteo, idSoil, idMeteo, CELLSIZE, fileName, error))
    {
//        GisObject* newObject = new(GisObject);
//        newObject->setShapeFile(QString::fromStdString(ucm->getFilepath()), ucm);
//        this->objectList.push_back(newObject);
//        return true;
    }
    else
    {
        return false;
    }
}
