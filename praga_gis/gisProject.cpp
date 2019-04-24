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


GisProject::GisProject()
{
}


bool GisProject::loadRaster(QString myFileName)
{
    std::string* myError = new std::string();
    std::string fileName = myFileName.left(myFileName.length()-4).toStdString();

    gis::Crit3DRasterGrid *myRaster = new(gis::Crit3DRasterGrid);
    if (! gis::readEsriGrid(fileName, myRaster, myError))
    {
        qDebug("Load raster failed!");
        return (false);
    }

    this->rasterList.push_back(myRaster);

    setDefaultDTMScale(this->rasterList.back()->colorScale);

    return (true);
}


QString getFileName(QString fileNameComplete)
{
    QString c;
    QString fileName = "";
    for (int i = fileNameComplete.length()-1; i >= 0; i--)
    {
        c = fileNameComplete.mid(i,1);
        if ((c != "\\") && (c != "/"))
            fileName = c + fileName;
        else
            return fileName;
    }
    return fileName;
}
