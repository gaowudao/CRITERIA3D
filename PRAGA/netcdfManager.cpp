#include <netcdf.h>
#include <QtDebug>

#include "netcdfManager.h"

namespace NetCDF
{
    bool provaNetCDF(QString fileName)
    {
        int ncId, retval;
        char name[NC_MAX_NAME+1];
        char varName[NC_MAX_NAME+1];
        char typeName[NC_MAX_NAME+1];
        char *valueStr;
        int valueInt;
        double value;
        size_t lenght;
        nc_type ncTypeId;

        // Open the file. NC_NOWRITE tells netCDF we want read-only access
        if ((retval = nc_open(fileName.toStdString().data(), NC_NOWRITE, &ncId)))
          qDebug() << nc_strerror(retval);

        // NC_INQ tells how many netCDF dimensions, variables and global attributes are in
        // the file, also the dimension id of the unlimited dimension, if there is one.
        int nrDimensions, nrVariables, nrGlobalAttributes, unlimDimensionId;
        if ((retval = nc_inq(ncId, &nrDimensions, &nrVariables, &nrGlobalAttributes, &unlimDimensionId)))
            qDebug() << nc_strerror(retval);

        qDebug() << "\n" ;
        for (int a = 0; a <nrGlobalAttributes; a++)
        {
           nc_inq_attname(ncId, NC_GLOBAL, a, name);
           nc_inq_attlen(ncId, NC_GLOBAL, name, &lenght);
           valueStr = (char *) calloc(lenght +1, sizeof(char));
           nc_get_att_text(ncId, NC_GLOBAL, name, valueStr);
           qDebug() << name << "=" << valueStr;
       }

       int varDimIds[NC_MAX_VAR_DIMS];
       int nrVarDimensions, nrVarAttributes;

       qDebug() << "\nDimensions: ";
       for (int i = 0; i < nrDimensions; i++)
       {
           nc_inq_dim(ncId, i, name, &lenght);
           qDebug() << i << name << "\t values:" << lenght;
       }

       qDebug() << "\nVariables: ";
       for (int v = 0; v < nrVariables; v++)
       {
           nc_inq_var(ncId, v, varName, &ncTypeId, &nrVarDimensions, varDimIds, &nrVarAttributes);
           nc_inq_type(ncId, ncTypeId, typeName, &lenght);

           QStringList dimList;
           for (int d = 0; d < nrVarDimensions; d++)
           {
               nc_inq_dim(ncId, varDimIds[d], name, &lenght);
               dimList.append(name);
           }

           qDebug() << v << varName << "\t\t type:" << typeName
                            << "\t dims:" << nrVarDimensions << dimList;

           /*
           for (int a = 0; a < nrVarAttributes; a++)
           {
              nc_inq_attname(ncId, v, a, name);
              nc_inq_atttype(ncId, v, name, &ncTypeId);
              if (ncTypeId == NC_CHAR)
              {
                  nc_inq_attlen(ncId, v, name, &lenght);
                  valueStr = (char *) calloc(lenght +1, sizeof(char));
                  nc_get_att_text(ncId, v, name, valueStr);
                  qDebug() << name << "=" << valueStr;
              }
              else if (ncTypeId == NC_INT)
              {
                  nc_get_att(ncId, v, name, &valueInt);
                  qDebug() << name << "=" << valueInt;
              }
              else if (ncTypeId == NC_DOUBLE)
              {
                  nc_get_att(ncId, v, name, &value);
                  qDebug() << name << "=" << value;
              }
          }*/
       }

       // Close the file, freeing all resources
       if ((retval = nc_close(ncId)))
          qDebug() << nc_strerror(retval);

       return true;
    }


}
