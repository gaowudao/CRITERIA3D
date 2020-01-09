#include "zonalStatistic.h"
#include "shapeToRaster.h"
#include "commonConstants.h"
#include <float.h>
#include <algorithm>
#include <math.h>

bool zonalStatisticsShape(Crit3DShapeHandler* shapeRef, Crit3DShapeHandler* shapeVal, gis::Crit3DRasterGrid *rasterRef, gis::Crit3DRasterGrid *rasterVal, std::string valField, opType type, std::string* error)
{

    // check shape type
    if ( shapeRef->getTypeString() != shapeVal->getTypeString() || shapeRef->getTypeString() != "2D Polygon" )
    {
        *error = "shape type error: not 2D Polygon type" ;
        return false;
    }
    // check proj
    if (shapeRef->getIsWGS84() == false || shapeVal->getIsWGS84() == false)
    {
        *error = "proj error: not WGS84" ;
        return false;
    }
    // check utm zone
    if (shapeRef->getUtmZone() != shapeVal->getUtmZone())
    {
        *error = "utm zone error: different utm zones" ;
        return false;
    }
    //check if valField exists
    int fieldIndex = shapeVal->getDBFFieldIndex(valField.c_str());
    if (fieldIndex == -1)
    {
        *error = shapeVal->getFilepath() + "has not field called " + valField.c_str();
        return false;
    }

    int nrRefShapes = shapeRef->getShapeCount();

    // add new field to shapeRef
    DBFFieldType fieldType = shapeVal->getFieldType(fieldIndex);
    shapeRef->addField(valField.c_str(), fieldType, shapeVal->nWidthField(fieldIndex), shapeVal->nDecimalsField(fieldIndex));

    // LC da fare restyle codice affinchè gestisca i 3 casi
    std::vector<std::string> varFieldVectorString;
    std::vector<int> varFieldVectorInt;
    std::vector<double> varFieldVectorDouble;
    size_t varFieldVectorSize = 0;


    if (fieldType == FTString)
    {
        // fill varFieldVector
        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            std::string value = shapeVal->readStringAttribute(record,fieldIndex);
            if (std::find (varFieldVectorString.begin(), varFieldVectorString.end(), value) == varFieldVectorString.end())
            {
                varFieldVectorString.push_back(value);
            }
        }
        varFieldVectorSize = varFieldVectorString.size();

        // assign varFieldVector index to each pixel of rasterVal
        for (int row = 0; row < rasterVal->header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal->header->nrCols; col++)
            {
                int shape = int(rasterVal->value[row][col]);
                if (shape!= NODATA)
                {
                    std::string valueField = shapeVal->readStringAttribute(shape,fieldIndex);
                    int vectorFieldPos = int(std::distance(varFieldVectorString.begin(), std::find (varFieldVectorString.begin(), varFieldVectorString.end(), valueField)));
                    //replace value
                    rasterVal->value[row][col] = float(vectorFieldPos);
                }
            }
        }
    }
    else if (fieldType == FTInteger)
    {

        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            int value = shapeVal->readIntAttribute(record,fieldIndex);
            if (std::find (varFieldVectorInt.begin(), varFieldVectorInt.end(), value) == varFieldVectorInt.end())
            {
                varFieldVectorInt.push_back(value);
            }
        }

        varFieldVectorSize = varFieldVectorInt.size();

        for (int row = 0; row < rasterVal->header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal->header->nrCols; col++)
            {
                int shape = int(rasterVal->value[row][col]);
                if (shape!= NODATA)
                {
                    int valueField = shapeVal->readIntAttribute(shape,fieldIndex);
                    int vectorFieldPos = int(std::distance(varFieldVectorInt.begin(), std::find (varFieldVectorInt.begin(), varFieldVectorInt.end(), valueField)));
//                    //replace value
                    rasterVal->value[row][col] = float(vectorFieldPos);
                }
            }
        }
    }
    else if (fieldType == FTDouble)
    {

        for (int record = 0; record < shapeVal->getDBFRecordCount(); record++)
        {
            double value = shapeVal->readDoubleAttribute(record,fieldIndex);
            if (std::find (varFieldVectorDouble.begin(), varFieldVectorDouble.end(), value) == varFieldVectorDouble.end())
            {
                varFieldVectorDouble.push_back(value);
            }
        }

        varFieldVectorSize = varFieldVectorDouble.size();

        for (int row = 0; row < rasterVal->header->nrRows; row++)
        {
            for (int col = 0; col < rasterVal->header->nrCols; col++)
            {
                int shape = int(rasterVal->value[row][col]);
                if (shape!= NODATA)
                {
                    double valueField = shapeVal->readDoubleAttribute(shape,fieldIndex);
                    int vectorFieldPos = std::distance(varFieldVectorDouble.begin(), std::find (varFieldVectorDouble.begin(), varFieldVectorDouble.end(), valueField));
//                    //replace value
                    rasterVal->value[row][col] = vectorFieldPos;
                }
            }
        }
    }

    // analysis matrix
    std::vector <std::vector<int> > matrix(nrRefShapes, std::vector<int>(varFieldVectorSize, 0));


    for (int row = 0; row < rasterRef->header->nrRows; row++)
    {
        for (int col = 0; col < rasterRef->header->nrCols; col++)
        {
            int refValue = int(rasterRef->value[row][col]);
            int valValue = int(rasterVal->value[row][col]);

            if ( refValue != NODATA && valValue != NODATA)
            {
                matrix[refValue][valValue] = matrix[refValue][valValue] + 1;
            }
        }
    }

    // save right matrix element for each shape
    std::vector<int> indexVector;

    if (type == MAJORITY)
    {
        for (int row = 0; row < nrRefShapes; row++)
        {
            indexVector.push_back(0);
            for (int col = 0; col < varFieldVectorSize; col++)
            {
                if (matrix[row][col] > indexVector.at(row))
                {
                    indexVector.at(row) = col;
                }
            }
        }
    }
    else if (type == MIN)
    {
        for (int row = 0; row < nrRefShapes; row++)
        {
            // TO DO
        }
    }
    else if (type == MAX)
    {
        for (int row = 0; row < nrRefShapes; row++)
        {
            // TO DO
        }
    }
    else if (type == AVG)
    {
        for (int row = 0; row < nrRefShapes; row++)
        {
            // TO DO
        }
    }

    // save value of the new field
    // each row of matrix is a shape of shapeRef
    for (int shapeIndex = 0; shapeIndex < nrRefShapes; shapeIndex++)
    {
        if (fieldType == FTString)
        {
            std::string varFieldFound = varFieldVectorString[indexVector[shapeIndex]];
            shapeRef->writeStringAttribute(shapeIndex, shapeRef->getDBFFieldIndex(valField.c_str()), varFieldFound.c_str());
        }
        else if (fieldType == FTInteger)
        {
            int varFieldFound = varFieldVectorInt[indexVector[shapeIndex]];
            shapeRef->writeIntAttribute(shapeIndex, shapeRef->getDBFFieldIndex(valField.c_str()), varFieldFound);
        }
        else if (fieldType == FTDouble)
        {
            double varFieldFound = varFieldVectorDouble[indexVector[shapeIndex]];
            shapeRef->writeDoubleAttribute(shapeIndex, shapeRef->getDBFFieldIndex(valField.c_str()), varFieldFound);
        }
    }

    // close and re-open to write also the last shape
    shapeRef->close();
    shapeRef->open(shapeRef->getFilepath());
    return true;
}


// TO DO
// zonalStatisticRaster (da Praga Vb)
