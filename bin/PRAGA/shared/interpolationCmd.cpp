#include "interpolationCmd.h"
#include "interpolation.h"
#include "formInfo.h"
#include "gis.h"


void Crit3DProxyGridSeries::initialize()
{
    gridName.clear();
    gridYear.clear();
    proxyName = "";
}

Crit3DProxyGridSeries::Crit3DProxyGridSeries()
{
    initialize();
}

Crit3DProxyGridSeries::Crit3DProxyGridSeries(QString name_)
{
    initialize();
    proxyName = name_;
}

void Crit3DProxyGridSeries::addGridToSeries(QString name_, unsigned year_)
{
    gridName.push_back(name_);
    gridYear.push_back(year_);
}

bool interpolationRaster(std::vector <Crit3DInterpolationDataPoint> &myPoints, Crit3DInterpolationSettings* mySettings,
                        gis::Crit3DRasterGrid* myGrid, const gis::Crit3DRasterGrid& raster, meteoVariable myVar, bool showInfo)
{
    if (! myGrid->initializeGrid(raster))
        return false;

    FormInfo myInfo;
    int infoStep;
    QString infoStr;

    if (showInfo)
    {
        infoStr = "Interpolation on DEM...";
        infoStep = myInfo.start(infoStr, myGrid->header->nrRows);
    }

    float myX, myY;
    std::vector <float> proxyValues;
    proxyValues.resize(unsigned(mySettings->getProxyNr()));

    for (long myRow = 0; myRow < myGrid->header->nrRows ; myRow++)
    {
        if (showInfo)
            myInfo.setValue(myRow);

        for (long myCol = 0; myCol < myGrid->header->nrCols; myCol++)
        {
            gis::getUtmXYFromRowColSinglePrecision(*myGrid, myRow, myCol, &myX, &myY);
            float myZ = raster.value[myRow][myCol];
            if (int(myZ) != int(myGrid->header->flag))
            {
                if (getUseDetrendingVar(myVar)) getProxyValuesXY(myX, myY, mySettings, proxyValues);
                myGrid->value[myRow][myCol] = interpolate(myPoints, mySettings, myVar, myX, myY, myZ, proxyValues, true);
            }
        }
    }

    if (showInfo) myInfo.close();

    if (! gis::updateMinMaxRasterGrid(myGrid))
        return false;

    return true;
}
