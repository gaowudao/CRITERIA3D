#include "interpolationCmd.h"
#include "interpolation.h"
#include "formRunInfo.h"


bool loadProxyGrid(Crit3DProxy* myProxy)
{
    std::string* myError = new std::string();
    std::string gridName = myProxy->getGridName();
    gis::Crit3DRasterGrid* myGrid = myProxy->getGrid();
    return (gis::readEsriGrid(gridName, myGrid, myError));
}

bool interpolationRaster(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                        gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid& myDTM, meteoVariable myVar, bool showInfo)
{
    if (! myGrid->initializeGrid(myDTM))
        return (false);

    formRunInfo myInfo;
    int infoStep;
    QString infoStr;

    if (showInfo)
    {
        infoStr = "Interpolation on DEM...";
        infoStep = myInfo.start(infoStr, myGrid->header->nrRows);
    }

    float myX, myY;

    for (long myRow = 0; myRow < myGrid->header->nrRows ; myRow++)
    {
        if (showInfo)
            myInfo.setValue(myRow);

        for (long myCol = 0; myCol < myGrid->header->nrCols; myCol++)
        {
            gis::getUtmXYFromRowColSinglePrecision(*myGrid, myRow, myCol, &myX, &myY);
            float myZ = myDTM.value[myRow][myCol];
            if (myZ != myGrid->header->flag)
                myGrid->value[myRow][myCol] = interpolate(myPoints, mySettings, myVar, myX, myY, myZ, getProxyValuesXY(myX, myY, mySettings), true);
        }
    }

    if (showInfo) myInfo.close();

    if (! gis::updateMinMaxRasterGrid(myGrid))
        return (false);

    return (true);
}
