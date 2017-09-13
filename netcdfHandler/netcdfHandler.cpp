#include <iostream>
#include <sstream>
#include <vector>
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
    this->initialize(NODATA);
}


void NetCDFHandler::initialize(int myUtmZone)
{
    utmZone = myUtmZone;
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
    isStandardTime = false;
    timeType = NODATA;

    x = NULL;
    y = NULL;
    time = NULL;

    dataGrid.freeGrid();
    dimensions.clear();
    variables.clear();
}


int NetCDFHandler::getDimensionIndex(char* dimName)
{
    for (unsigned int i = 0; i < dimensions.size(); i++)
    {
        if (dimensions[i].name == std::string(dimName))
            return i;
    }

    return NODATA;
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
    int ncId, retval;
    char name[NC_MAX_NAME+1];
    char attrName[NC_MAX_NAME+1];
    char varName[NC_MAX_NAME+1];
    char typeName[NC_MAX_NAME+1];
    char *valueStr;
    int valueInt;
    double value;
    size_t lenght;
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
    for (int a = 0; a <nrGlobalAttributes; a++)
    {
       nc_inq_attname(ncId, NC_GLOBAL, a, attrName);
       nc_inq_attlen(ncId, NC_GLOBAL, attrName, &lenght);
       valueStr = (char *) calloc(lenght +1, sizeof(char));
       nc_get_att_text(ncId, NC_GLOBAL, attrName, valueStr);

       *buffer << attrName << " = " << valueStr << endl;
   }

   // DIMENSIONS
   int varDimIds[NC_MAX_VAR_DIMS];
   int nrVarDimensions, nrVarAttributes;

   *buffer << "\nDimensions: " << endl;
   for (int i = 0; i < nrDimensions; i++)
   {
       nc_inq_dim(ncId, i, name, &lenght);

       dimensions.push_back(NetCDFVariable(name, i, NODATA));

       if (lowerCase(string(name)) == "time")
       {
           nrTime = int(lenght);
       }
       else if (lowerCase(string(name)) == "x")
       {
           nrX = int(lenght);
           isLatLon = false;
       }
       else if (lowerCase(string(name)) == "y")
       {
           nrY = int(lenght);
           isLatLon = false;
       }
       else if (lowerCase(string(name)) == "lat" || lowerCase(string(name)) == "latitude")
       {
           nrLat = int(lenght);
           isLatLon = true;
       }
       else if (lowerCase(string(name)) == "lon" || lowerCase(string(name)) == "longitude")
       {
           nrLon = int(lenght);
           isLatLon = true;
       }

       *buffer << i << " - " << name << "\t values: " << lenght << endl;
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
       nc_inq_type(ncId, ncTypeId, typeName, &lenght);

       // is Variable?
       if (nrVarDimensions > 1)
       {
            variables.push_back(NetCDFVariable(varName, v, ncTypeId));
       }
       else
       {
            int i = getDimensionIndex(varName);
            if (i != NODATA)
                dimensions[i].type = ncTypeId;
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
           nc_inq_dim(ncId, varDimIds[d], name, &lenght);
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
                nc_inq_attlen(ncId, v, attrName, &lenght);
                valueStr = (char *) calloc(lenght +1, sizeof(char));
                nc_get_att_text(ncId, v, attrName, valueStr);
                *buffer << attrName << " = " << valueStr << endl;

                if (v == idTime)
                {
                    if ((lowerCase(string(attrName)) == "units")
                       && (lowerCase(string(valueStr).substr(0, 18)) == "seconds since 1970"))
                           isStandardTime = true;
                }
                if (lowerCase(string(attrName)) == "long_name")
                    setVarLongName(varName, valueStr);

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
            float* lat = (float*) calloc(nrLat, sizeof(float));
            float* lon = (float*) calloc(nrLon, sizeof(float));

            if (retval = nc_get_var_float(ncId, idLon, lon))
                *buffer << "\nERROR in reading longitude:" << nc_strerror(retval);

            if (retval = nc_get_var_float(ncId, idLat, lat))
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

            latLonHeader.llCorner->longitude = lon[0];
            latLonHeader.dx = (lon[1]-lon[0]);

            if (lat[1] > lat[0])
            {
                latLonHeader.llCorner->latitude = lat[0];
                latLonHeader.dy = (lat[1]-lat[0]);
            }
            else
            {
                latLonHeader.llCorner->latitude = lat[nrLat-1];
                latLonHeader.dy = (lat[0]-lat[1]);
            }

            latLonHeader.flag = NODATA;

            dataGrid.header->convertFromLatLon(latLonHeader);
            dataGrid.initializeGrid(0);
        }
    }
    else
    {
        if (idX != NODATA && idY != NODATA)
        {
            x = (float*) calloc(nrX, sizeof(float));
            if (retval = nc_get_var_float(ncId, idX, x))
            {
                *buffer << "\nERROR in reading x: " << nc_strerror(retval);
                nc_close(ncId);
                return false;
            }

            y = (float*) calloc(nrY, sizeof(float));
            if (retval = nc_get_var_float(ncId, idY, y))
            {
                *buffer << "\nERROR in reading y: " << nc_strerror(retval);
                nc_close(ncId);
                return false;
            }

            if ((x[1]-x[0]) != (y[1]-y[0]))
                *buffer << "\nWarning! dx != dy" << endl;

            dataGrid.header->cellSize = x[1]-x[0];
            dataGrid.header->llCorner->x = x[0] - dataGrid.header->cellSize*0.5;
            dataGrid.header->llCorner->y = y[0] - dataGrid.header->cellSize*0.5;

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
        time = (double*) calloc(nrTime, sizeof(double));

        if (timeType == NC_DOUBLE)
            retval = nc_get_var(ncId, idTime, time);

        else if (timeType == NC_FLOAT)
        {
            float* floatTime = (float*) calloc(nrTime, sizeof(float));
            retval = nc_get_var_float(ncId, idTime, floatTime);
            for (int i = 0; i < nrTime; i++)
                time[i] = floatTime[i];
        }
    }

    *buffer << endl << "first date: " << getDateTimeStr(0) << endl;
    *buffer << "last date: " << getDateTimeStr(nrTime-1) << endl;

    isLoaded = true;

    *buffer << endl << "VARIABLES list:" << endl;
    for (unsigned int i = 0; i < variables.size(); i++)
        *buffer << variables[i].getVarName() << endl;


    // CLOSE file, freeing all resources
    nc_close(ncId);

   return true;
}


bool NetCDFHandler::exportDataSeries(int idVar, gis::Crit3DGeoPoint geoPoint, time_t firstTime, time_t lastTime, stringstream *buffer)
{
    //check
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


    //find row, col of point

    return true;
}
