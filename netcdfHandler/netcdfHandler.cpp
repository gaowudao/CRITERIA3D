#include <netcdf.h>
#include <iostream>
#include <vector>

#include "netcdfHandler.h"

using namespace std;


namespace NetCDF
{
    bool provaNetCDF(string fileName)
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
        int idDimX, idDimY, nrX, nrY;
        string nameDimX, nameDimY;

        //NC_NOWRITE tells netCDF we want read-only access
        if ((retval = nc_open(fileName.data(), NC_NOWRITE, &ncId)))
            cout << nc_strerror(retval);

        // NC_INQ tells how many netCDF dimensions, variables and global attributes are in
        // the file, also the dimension id of the unlimited dimension, if there is one.
        int nrDimensions, nrVariables, nrGlobalAttributes, unlimDimensionId;
        if ((retval = nc_inq(ncId, &nrDimensions, &nrVariables, &nrGlobalAttributes, &unlimDimensionId)))
            cout << nc_strerror(retval);

        cout << endl ;

        // Global attributes
        for (int a = 0; a <nrGlobalAttributes; a++)
        {
           nc_inq_attname(ncId, NC_GLOBAL, a, name);
           nc_inq_attlen(ncId, NC_GLOBAL, name, &lenght);
           valueStr = (char *) calloc(lenght +1, sizeof(char));
           nc_get_att_text(ncId, NC_GLOBAL, name, valueStr);

           cout << name << " = " << valueStr << endl;
       }

       int varDimIds[NC_MAX_VAR_DIMS];
       int nrVarDimensions, nrVarAttributes;

       cout << "\nDimensions: " << endl;
       for (int i = 0; i < nrDimensions; i++)
       {
           nc_inq_dim(ncId, i, name, &lenght);

           if (string(name) == "x" || string(name) == "lon" || string(name) == "longitude")
           {
               nrX = int(lenght);
               nameDimX = string(name);
           }
           if (string(name) == "y" || string(name) == "lat" || string(name) == "latitude")
           {
               nrY = int(lenght);
               nameDimY = string(name);
           }

           cout << i << " - " << name << "\t values: " << lenght << endl;
       }

       cout <<"\n(x,y) = "<<nrX << "," <<nrY << endl;

       cout << "\nVariables: " << endl;
       for (int v = 0; v < nrVariables; v++)
       {
           nc_inq_var(ncId, v, varName, &ncTypeId, &nrVarDimensions, varDimIds, &nrVarAttributes);
           nc_inq_type(ncId, ncTypeId, typeName, &lenght);

           if (string(varName) == nameDimX)
               idDimX = v;
           if (string(varName) == nameDimY)
               idDimY = v;

           cout << endl << v << " - " << varName << "\t type: " << typeName << "\t dims: ";
           for (int d = 0; d < nrVarDimensions; d++)
           {
               nc_inq_dim(ncId, varDimIds[d], name, &lenght);
               cout << name << " ";
           }
           cout << endl;

           for (int a = 0; a < nrVarAttributes; a++)
           {
                nc_inq_attname(ncId, v, a, name);
                nc_inq_atttype(ncId, v, name, &ncTypeId);
                if (ncTypeId == NC_CHAR)
                {
                    nc_inq_attlen(ncId, v, name, &lenght);
                    valueStr = (char *) calloc(lenght +1, sizeof(char));
                    nc_get_att_text(ncId, v, name, valueStr);
                    cout << name << " = " << valueStr << endl;
                }
                else if (ncTypeId == NC_INT)
                {
                    nc_get_att(ncId, v, name, &valueInt);
                    cout << name << " = " << valueInt << endl;
                }
                else if (ncTypeId == NC_DOUBLE)
                {
                    nc_get_att(ncId, v, name, &value);
                    cout << name << " = " << value << endl;
                }
            }
        }

        float* x = (float*) calloc(nrX, sizeof(float));
        if (retval = nc_get_var_float(ncId, idDimX, x))
            cout << nc_strerror(retval);

        float* y = (float*) calloc(nrY, sizeof(float));
        if (retval = nc_get_var_float(ncId, idDimY, y))
            cout << nc_strerror(retval);

        if (nameDimX == "x")
        {
            float* lat = (float*) calloc(nrY*nrX, sizeof(float));
            float* lon = (float*) calloc(nrY*nrX, sizeof(float));

            if (retval = nc_get_var_float(ncId, 4, lon))
                cout << "error in reading longitude:" << nc_strerror(retval);

            if (retval = nc_get_var_float(ncId, 5, lat))
                cout << "error in reading latitude:" << nc_strerror(retval);

            for (int row = 0; row < nrY; row+=2)
            {
                cout << lat[row*nrX+row] << ",";
                cout << lon[row*nrX+row] << "  ";
            }

            cout << "HERE";
        }


        // CLOSE file, freeing all resources
        if ((retval = nc_close(ncId)))
            cout << nc_strerror(retval);

       return true;
    }

}
