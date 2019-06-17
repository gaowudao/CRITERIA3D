#include "gisObject.h"


GisObject::GisObject()
{
    this->initialize();
}


void GisObject::initialize()
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


gis::Crit3DRasterGrid* GisObject::getRaster()
{
    return this->rasterPtr;
}


Crit3DShapeHandler* GisObject::getShapeHandler()
{
    return this->shapePtr;
}


void GisObject::close()
{
    if (this->type == gisObjectRaster)
    {
        delete rasterPtr;
    }
    else if (this->type == gisObjectShape)
    {
        shapePtr->close();
    }

    this->initialize();
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

