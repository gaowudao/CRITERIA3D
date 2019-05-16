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


QString GisObject::getFileNameWithPath() const
{
    return fileNameWithPath;
}

void GisObject::setFileNameWithPath(const QString &value)
{
    fileNameWithPath = value;
}

GisObject::GisObject()
{
    this->type = gisObjectNone;
    this->fileName = "";
    this->fileNameWithPath = "";
    this->isSelected = true;

    this->rasterPtr = nullptr;
    this->shapePtr = nullptr;
}


void GisObject::setRaster(QString filename, gis::Crit3DRasterGrid* rasterPtr)
{
    this->type = gisObjectRaster;
    this->fileName = filename;
    this->isSelected = true;

    this->rasterPtr = rasterPtr;
}


void GisObject::setShape(QString filename, Crit3DShapeHandler* shapePtr)
{
    this->type = gisObjectShape;
    this->fileName = filename;
    this->isSelected = true;

    this->shapePtr = shapePtr;
}


GisProject::GisProject()
{
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

    GisObject* newObject = new(GisObject);
    newObject->setRaster(getFileName(fileNameWithPath), myRaster);
    this->objectList.push_back(newObject);

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

    GisObject* newObject = new(GisObject);
    newObject->setShape(getFileName(fileNameWithPath), myShape);
    newObject->setFileNameWithPath(fileNameWithPath);
    this->objectList.push_back(newObject);

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
