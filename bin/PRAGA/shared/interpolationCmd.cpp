#include "interpolationCmd.h"
#include "interpolation.h"
#include "formInfo.h"
#include "gis.h"


void Crit3DProxyGridSeries::initialize()
{
    gridName.clear();
    grid.clear();
    gridYear.clear();
}

std::vector<gis::Crit3DRasterGrid *> Crit3DProxyGridSeries::getGrid() const
{
    return grid;
}

void Crit3DProxyGridSeries::setGrid(const std::vector<gis::Crit3DRasterGrid *> &value)
{
    grid = value;
}

Crit3DProxyGridSeries::Crit3DProxyGridSeries()
{
    initialize();
}

void Crit3DProxyGridSeries::addGridToSeries(std::string name_, int year_)
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
