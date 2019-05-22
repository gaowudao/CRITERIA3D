#include "unitCropMap.h"

bool zonalStatistics(Crit3DShapeHandler* shapeRef, Crit3DShapeHandler* shapeVal, std::string valField, DBFFieldType fieldType, int cellSize, opType type)
{

    // check shape type
    if ( shapeRef->getTypeString() != shapeVal->getTypeString() || shapeRef->getTypeString() != "2D Polygon" )
    {
        return false;
    }
    // check proj
    if (shapeRef->getIsWGS84() == false || shapeVal->getIsWGS84() == false)
    {
        return false;
    }
    // check utm zone
    if (shapeRef->getUtmZone() != shapeVal->getUtmZone())
    {
        return false;
    }

    return true;


}


