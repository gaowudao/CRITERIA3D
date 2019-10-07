/*!
    \copyright 2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by ARPAE Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/


#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

#include <netcdf.h>

#include "commonConstants.h"
#include "netcdfHandler.h"

using namespace std;


string lowerCase(string myStr)
{
    transform(myStr.begin(), myStr.end(), myStr.begin(), ::tolower);
    return myStr;
}


NetCDFVariable::NetCDFVariable()
{
    name = "";
    longName = "";
    id = NODATA;
    type = NODATA;
}


NetCDFVariable::NetCDFVariable(char* myName, int myId, int myType)
{
    name = myName;
    longName = myName;
    id = myId;
    type = myType;
}

std::string NetCDFVariable::getVarName()
{
    if (longName.size() < 30)
        return longName;
    else
        return name;
}


NetCDFHandler::NetCDFHandler()
{
    ncId = NODATA;
    this->clear();
}


void NetCDFHandler::clear()
{
    // CLOSE file, freeing all resources
    if (ncId != NODATA) nc_close(ncId);
    ncId = NODATA;

    utmZone = NODATA;

    nrX = NODATA;
    nrY = NODATA;
    nrLat = NODATA;
    nrLon = NODATA;
    nrTime = NODATA;

    idX = NODATA;
    idY = NODATA;
    idLat = NODATA;
    idLon = NODATA;
    idTime = NODATA;

    isLoaded = false;
    isLatLon = false;
    isLatDecreasing = false;
    isStandardTime = false;
    timeType = NODATA;

    x = nullptr;
    y = nullptr;
    time = nullptr;

    dataGrid.clear();
    dimensions.clear();
    variables.clear();
}


void NetCDFHandler::initialize(int _utmZone)
{
    this->clear();
    utmZone = _utmZone;
}


int NetCDFHandler::getDimensionIndex(char* dimName)
{
    for (unsigned int i = 0; i < dimensions.size(); i++)
    {
        if (dimensions[i].name == std::string(dimName))
            return int(i);
    }

    return NODATA;
}


std::string NetCDFHandler::getVarName(int idVar)
{
    for (unsigned int i = 0; i < variables.size(); i++)
        if (variables[i].id == idVar)
            return variables[i].longName;

    return "";
}


bool NetCDFHandler::setVarLongName(char* varName, char* varLongName)
{
    for (unsigned int i = 0; i < variables.size(); i++)
    {
        if (variables[i].name == std::string(varName))
        {
            variables[i].longName = varLongName;
            return true;
        }
    }

    return false;
}


bool NetCDFHandler::isPointInside(gis::Crit3DGeoPoint geoPoint)
{
    if (! isLoaded) return false;

    if (isLatLon)
    {
        return geoPoint.isInsideGrid(latLonHeader);
    }
    else
    {
        gis::Crit3DUtmPoint utmPoint;
        gis::getUtmFromLatLon(utmZone, geoPoint, &utmPoint);
        return utmPoint.isInsideGrid(*(dataGrid.header));
    }
}

std::string NetCDFHandler::getDateTimeStr(int timeIndex)
{
    // check
    if (! isStandardTime)
        return "ERROR: time is not standard (seconds since 1970)";

    if (timeIndex < 0 || timeIndex >= nrTime)
        return "ERROR: time index out of range";

    time_t myTime = time_t(time[timeIndex]);
    std::stringstream s;
    s << std::put_time(std::gmtime(&myTime), "%Y-%m-%d %H:%M:%S");

    return s.str();
}


time_t NetCDFHandler::getTime(int timeIndex)
{
    return time_t(time[timeIndex]);
}


bool NetCDFHandler::readProperties(string fileName, stringstream *buffer)
{
    int retval;
    //char name[NC_MAX_NAME+1];
    char* name = new char[NC_MAX_NAME+1];
    char* attrName = new char[NC_MAX_NAME+1];
    char* varName = new char[NC_MAX_NAME+1];
    char* typeName = new char[NC_MAX_NAME+1];
    char* valueStr;
    int valueInt;
    double value;
    size_t length;
    nc_type ncTypeId;

    //NC_NOWRITE tells netCDF we want read-only access
    if ((retval = nc_open(fileName.data(), NC_NOWRITE, &ncId)))
    {
        *buffer << nc_strerror(retval) << endl;
        return false;
    }

    // NC_INQ tells how many netCDF dimensions, variables and global attributes are in
    // the file, also the dimension id of the unlimited dimension, if there is one.
    int nrDimensions, nrVariables, nrGlobalAttributes, unlimDimensionId;
    if ((retval = nc_inq(ncId, &nrDimensions, &nrVariables, &nrGlobalAttributes, &unlimDimensionId)))
    {
        *buffer << nc_strerror(retval) << endl;
        return false;
    }

    // GLOBAL ATTRIBUTES
    *buffer << fileName << endl << endl;
    *buffer << "Global attributes:" << endl;

    for (int a = 0; a < nrGlobalAttributes; a++)
    {
        nc_inq_attname(ncId, NC_GLOBAL, a, attrName);
        nc_inq_attlen(ncId, NC_GLOBAL, attrName, &length);

        valueStr = new char[length+1];
        nc_get_att_text(ncId, NC_GLOBAL, attrName, valueStr);

        string myString = string(valueStr).substr(0, length);
        *buffer << attrName << " = " << myString << endl;

        delete [] valueStr;
   }

   // DIMENSIONS
   int varDimIds[NC_MAX_VAR_DIMS];
   int nrVarDimensions, nrVarAttributes;

   *buffer << "\nDimensions: " << endl;
   for (int i = 0; i < nrDimensions; i++)
   {
       nc_inq_dim(ncId, i, name, &length);

       dimensions.push_back(NetCDFVariable(name, i, NODATA));

       if (lowerCase(string(name)) == "time")
       {
           nrTime = int(length);
       }
       else if (lowerCase(string(name)) == "x")
       {
           nrX = int(length);
           isLatLon = false;
       }
       else if (lowerCase(string(name)) == "y")
       {
           nrY = int(length);
           isLatLon = false;
       }
       else if (lowerCase(string(name)) == "lat" || lowerCase(string(name)) == "latitude")
       {
           nrLat = int(length);
           isLatLon = true;
       }
       else if (lowerCase(string(name)) == "lon" || lowerCase(string(name)) == "longitude")
       {
           nrLon = int(length);
           isLatLon = true;
       }

       *buffer << i << " - " << name << "\t values: " << length << endl;
   }

   if (isLatLon)
       *buffer <<"\n(lon,lat) = "<<nrLon << "," <<nrLat << endl;
   else
       *buffer <<"\n(x,y) = "<<nrX << "," <<nrY << endl; 

   // VARIABLES
   *buffer << "\nVariables: " << endl;
   for (int v = 0; v < nrVariables; v++)
   {
       nc_inq_var(ncId, v, varName, &ncTypeId, &nrVarDimensions, varDimIds, &nrVarAttributes);
       nc_inq_type(ncId, ncTypeId, typeName, &length);

       // is Variable?
       if (nrVarDimensions > 1)
       {
            variables.push_back(NetCDFVariable(varName, v, ncTypeId));
       }
       else
       {
            int i = getDimensionIndex(varName);
            if (i != NODATA)
                dimensions[unsigned(i)].type = ncTypeId;
            else
                *buffer << endl << "ERRORE: dimensione non trovata: " << varName << endl;
       }

       if (lowerCase(string(varName)) == "time")
       {
           idTime = v;
           nc_inq_vartype(ncId, v, &timeType);
       }
       else if (lowerCase(string(varName)) == "x")
           idX = v;
       else if (lowerCase(string(varName)) == "y")
           idY = v;
       else if (lowerCase(string(varName)) == "lat" || lowerCase(string(varName)) == "latitude")
           idLat = v;
       else if (lowerCase(string(varName)) == "lon" || lowerCase(string(varName)) == "longitude")
           idLon = v;

       *buffer << endl << v  << "\t" << varName << "\t" << typeName << "\t dims: ";
       for (int d = 0; d < nrVarDimensions; d++)
       {
           nc_inq_dim(ncId, varDimIds[d], name, &length);
           *buffer << name << " ";
       }
       *buffer << endl;

       // ATTRIBUTES
       for (int a = 0; a < nrVarAttributes; a++)
       {
            nc_inq_attname(ncId, v, a, attrName);
            nc_inq_atttype(ncId, v, attrName, &ncTypeId);

            if (ncTypeId == NC_CHAR)
            {
                nc_inq_attlen(ncId, v, attrName, &length);
                valueStr = new char[length+1];
                nc_get_att_text(ncId, v, attrName, valueStr);

                string myString = string(valueStr).substr(0, length);
                *buffer << attrName << " = " << myString << endl;

                if (v == idTime)
                {
                    if ((lowerCase(string(attrName)) == "units"
                       && lowerCase(myString).substr(0, 18) == "seconds since 1970"))
                    {
                           isStandardTime = true;
                    }
                }
                if (lowerCase(string(attrName)) == "long_name")
                    setVarLongName(varName, valueStr);

                delete [] valueStr;
            }
            else if (ncTypeId == NC_INT)
            {
                nc_get_att(ncId, v, attrName, &valueInt);
                *buffer << attrName << " = " << valueInt << endl;
            }
            else if (ncTypeId == NC_DOUBLE)
            {
                nc_get_att(ncId, v, attrName, &value);
                *buffer << attrName << " = " << value << endl;
            }
        }
    }

    if (isLatLon)
    {
        if (idLat != NODATA && idLon != NODATA)
        {
            float* lat = new float[unsigned(nrLat)];
            float* lon = new float[unsigned(nrLon)];

            if ((retval = nc_get_var_float(ncId, idLon, lon)))
                *buffer << "\nERROR in reading longitude:" << nc_strerror(retval);

            if ((retval = nc_get_var_float(ncId, idLat, lat)))
                *buffer << "\nERROR in reading latitude:" << nc_strerror(retval);

            *buffer << endl << "lat:" << endl;
            for (int i = 0; i < nrLat; i++)
                *buffer << lat[i] << ", ";
            *buffer << endl << "lon:" << endl;
            for (int i = 0; i < nrLon; i++)
                *buffer << lon[i] << ", ";
            *buffer << endl;

            latLonHeader.nrRows = nrLat;
            latLonHeader.nrCols = nrLon;

            latLonHeader.llCorner->longitude = double(lon[0]);
            latLonHeader.dx = double(lon[nrLon-1] - lon[0]) / double(nrLon-1);

            if (lat[1] > lat[0])
            {
                latLonHeader.llCorner->latitude = double(lat[0]);
                latLonHeader.dy = double(lat[nrLat-1]-lat[0]) / double(nrLat-1);
            }
            else
            {
                latLonHeader.llCorner->latitude = double(lat[nrLat-1]);
                latLonHeader.dy = double(lat[0]-lat[nrLat-1]) / double(nrLat-1);
                isLatDecreasing = true;
            }

            latLonHeader.llCorner->longitude -= (latLonHeader.dx * 0.5);
            latLonHeader.llCorner->latitude -= (latLonHeader.dy * 0.5);

            latLonHeader.flag = NODATA;

            dataGrid.header->convertFromLatLon(latLonHeader);
            dataGrid.initializeGrid(0);
        }
    }
    else
    {
        if (idX != NODATA && idY != NODATA)
        {
            x = new float[unsigned(nrX)];
            if ((retval = nc_get_var_float(ncId, idX, x)))
            {
                *buffer << "\nERROR in reading x: " << nc_strerror(retval);
                nc_close(ncId);
                return false;
            }

            y = new float[unsigned(nrY)];
            if ((retval = nc_get_var_float(ncId, idY, y)))
            {
                *buffer << "\nERROR in reading y: " << nc_strerror(retval);
                nc_close(ncId);
                return false;
            }

            if ((x[1]-x[0]) != (y[1]-y[0]))
                *buffer << "\nWarning! dx != dy" << endl;

            dataGrid.header->cellSize = double(x[1]-x[0]);
            dataGrid.header->llCorner->x = double(x[0]) - dataGrid.header->cellSize*0.5;
            dataGrid.header->llCorner->y = double(y[0]) - dataGrid.header->cellSize*0.5;

            dataGrid.header->nrCols = nrX;
            dataGrid.header->nrRows = nrY;
            dataGrid.header->flag = NODATA;
            dataGrid.initializeGrid(0);
        }
        else
            *buffer << endl << "ERROR: missing x,y data" << endl;
    }

    // TIME
    if (isStandardTime)
    {
        time = new double[unsigned(nrTime)];

        if (timeType == NC_DOUBLE)
            retval = nc_get_var(ncId, idTime, time);

        else if (timeType == NC_FLOAT)
        {
            float* floatTime = new float[unsigned(nrTime)];
            retval = nc_get_var_float(ncId, idTime, floatTime);
            for (int i = 0; i < nrTime; i++)
                time[i] = double(floatTime[i]);
        }
    }

    *buffer << endl << "first date: " << getDateTimeStr(0) << endl;
    *buffer << "last date: " << getDateTimeStr(nrTime-1) << endl;

    isLoaded = true;

    *buffer << endl << "VARIABLES list:" << endl;
    for (unsigned int i = 0; i < variables.size(); i++)
        *buffer << variables[i].getVarName() << endl;

   return true;
}


bool NetCDFHandler::exportDataSeries(int idVar, gis::Crit3DGeoPoint geoPoint, time_t firstTime, time_t lastTime, stringstream *buffer)
{
    // check
    if (! isStandardTime)
    {
        *buffer << "Wrong time! Praga reads only POSIX standard (seconds since 1970-01-01)." << endl;
        return false;
    }
    if (! isPointInside(geoPoint))
    {
        *buffer << "Wrong Position!" << endl;
        return false;
    }

    if (firstTime < getFirstTime() || lastTime > getLastTime())
    {
        *buffer << "Time out of range!" << endl;
        return false;
    }

    // find row, col
    int row, col;
    if (isLatLon)
    {
        gis::getRowColFromLatLon(latLonHeader, geoPoint, &row, &col);
        if (!isLatDecreasing)
            row = (nrLat-1) - row;
    }
    else
    {
        gis::Crit3DUtmPoint utmPoint;
        gis::getUtmFromLatLon(utmZone, geoPoint, &utmPoint);
        gis::getRowColFromXY(*(dataGrid.header), utmPoint, &row, &col);
        row = (nrY -1) - row;
    }

    // find time indexes
    int t1 = NODATA;
    int t2 = NODATA;
    int i = 0;
    while ((i < nrTime) && (t1 == NODATA || t2 == NODATA))
    {
        if (time_t(time[i]) == firstTime)
            t1 = i;
        if (time_t(time[i]) == lastTime)
            t2 = i;
        i++;
    }

    // check time
    if  (t1 == NODATA || t2 == NODATA)
    {
        *buffer << "Time out of range!" << endl;
        return false;
    }

    // write variable
     *buffer << "variable: " << getVarName(idVar) <<endl;

    // write position
    if (isLatLon)
        *buffer << "lat: " << y[row] << "\tlon: " << x[col] << endl;
    else
        *buffer << "utm x: " << x[col] << "\tutm y: " << y[row] << endl;

    *buffer << endl;

    // write data
    size_t* index = new size_t[3];
    index[1] = size_t(row);
    index[2] = size_t(col);

    float value;
    for (int t = t1; t <= t2; t++)
    {
        index[0] = unsigned(t);
        nc_get_var1_float(ncId, idVar, index, &value);
        *buffer << getDateTimeStr(t) << "," << value << endl;
    }

    return true;
}
