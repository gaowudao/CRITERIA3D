#include <QDate>

#include "formInfo.h"
#include "gis.h"
#include "utilities.h"
#include "interpolation.h"
#include "interpolationCmd.h"

void Crit3DProxyGridSeries::initialize()
{
    gridName.clear();
    gridYear.clear();
    proxyName = "";
}

std::vector<QString> Crit3DProxyGridSeries::getGridName() const
{
    return gridName;
}

std::vector<unsigned> Crit3DProxyGridSeries::getGridYear() const
{
    return gridYear;
}

QString Crit3DProxyGridSeries::getProxyName() const
{
    return proxyName;
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

void interpolateProxyGridSeries(Crit3DProxyGridSeries mySeries, QDate myDate, gis::Crit3DRasterGrid* gridOut)
{
    std::string myError;
    std::vector <QString> gridNames = mySeries.getGridName();
    size_t nrGrids = gridNames.size();
    gridOut = new gis::Crit3DRasterGrid();

    if (nrGrids == 1)
    {
        gis::readEsriGrid(gridNames[0].toStdString(), gridOut, &myError);
        return;
    }

    unsigned first, second;

    for (second = 0; second < nrGrids; second++)
        if (unsigned(myDate.year()) <= mySeries.getGridYear()[second]) break;

    if (second == 0)
    {
        second = 1;
        first = 0;
    }
    else if (second == nrGrids)
    {
        first = unsigned(nrGrids) - 2;
        second = unsigned(nrGrids) - 1;
    }
    else
        first = second - 1;

    // load grids
    gis::Crit3DRasterGrid firstGrid, secondGrid;
    gis::readEsriGrid(gridNames[first].toStdString(), &firstGrid, &myError);
    gis::readEsriGrid(gridNames[second].toStdString(), &secondGrid, &myError);

    firstGrid.setMapTime(getCrit3DTime(myDate, 0));
    secondGrid.setMapTime(getCrit3DTime(myDate, 0));

    // use first as reference if different resolution when resampling
    /*if (! gis::compareGrids(firstGrid, secondGrid))
    {


    }

        If Not GIS.CompareGrids(myFirstMap, mySecondMap) Then
            Dim myTmpGrid As GIS.grid
            myTmpGrid = mySecondMap
            GIS.clearGrid mySecondMap
            GIS.resampleGridWithHeader myTmpGrid, mySecondMap, myFirstMap.header, _
                Definitions.ELAB_MEAN, 0, False
        End If

        'temporal interpolation
        If Not TemporalInterpolateMap(myYear, myFirstMap, mySecondMap, Interpolation.UrbanFractionMap, 0, 1) Then Exit Function
        'l'assegnazione alle stazioni va fatta alla massima risoluzione
        AssignGeoToStations Definitions.PROXYVAR_URBANFRACTION

        'aggregazione sul DEM
        Dim myUrbanMap As GIS.grid
        GIS.resampleGridWithHeader Interpolation.UrbanFractionMap, myUrbanMap, GIS.DEM.header, Definitions.ELAB_MEAN, 0, False
        GIS.clearGrid Interpolation.UrbanFractionMap
        Interpolation.UrbanFractionMap = myUrbanMap


        GIS.clearGrid myUrbanMap
        GIS.clearGrid myFirstMap
        GIS.clearGrid mySecondMap
        */
}

bool checkProxyGridSeries(Crit3DInterpolationSettings* mySettings, std::vector <Crit3DProxyGridSeries> mySeries, QDate myDate)
{
    unsigned i,j;

    for (i=0; i < mySettings->getProxyNr(); i++)
    {
        if (mySettings->getCurrentCombination()->getValue(i))
        {
            for (j=0; j < mySeries.size(); j++)
            {
                if (mySeries[j].getProxyName() == QString::fromStdString(mySettings->getProxyName(i)))
                {
                    if (mySeries[j].getGridName().size() > 0)
                    {
                        gis::Crit3DRasterGrid* gridOut = nullptr;
                        interpolateProxyGridSeries(mySeries[j], myDate, gridOut);
                        return true;
                    }

                }
            }
        }
    }

    return false;
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
