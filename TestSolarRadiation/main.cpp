#include <QCoreApplication>
#include "iostream"
#include "commonConstants.h"
#include "gis.h"
#include "radiationSettings.h"
#include "solarRadiation.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // INPUT
    std::string* fileName = new std::string("./data/dem_ravone");

    // GIS SETTINGS (default = UTM zone 32, timezone 1, time UTC)
    gis::Crit3DGisSettings* gisSettings = new gis::Crit3DGisSettings();
    gisSettings->utmZone = 32;
    gisSettings->timeZone = 1;

    // DATETIME
    Crit3DDate* myDate = new Crit3DDate(1,6, 2018);
    int myHour = 8;

    std::cout << "TEST Solar Radiation library" << std::endl;
    std::cout << "Compute a map of global solar irradiance (clear sky)" << std::endl;
    std::cout << "UTM zone: " << gisSettings->utmZone << std::endl;
    std::cout << "Date: " << myDate->toStdString() << " hour: " << myHour << " UTC" << std::endl;

    // READ DTM
    gis::Crit3DRasterGrid* dtm = new gis::Crit3DRasterGrid();
    std::string* error = new std::string();
    if (gis::readEsriGrid(*fileName, dtm, error))
        std::cout << "\nDTM loaded: " << *fileName << std::endl;
    else
    {
        std::cout << "Error in reading:" << *fileName << std::endl << *error << std::endl;
        return false;
    }

    // SET RADIATION SETTINGS
    Crit3DRadiationSettings* radSettings = new Crit3DRadiationSettings();
    radSettings->setGisSettings(gisSettings);

    // INITIALIZE RADIATION MAPS (deafult trasmissivity = 0.75)
    Crit3DRadiationMaps* radMaps = new Crit3DRadiationMaps(*dtm, *gisSettings);

    float mySeconds = HOUR_SECONDS * myHour;
    Crit3DTime* myTime = new Crit3DTime(*myDate, mySeconds);

    std::cout << "\nComputing..." << std::endl;

    // COMPUTE POTENTIAL GLOBAL RADIATION MAPS
    if (radiation::computeRadiationGridPresentTime(radSettings, *dtm, radMaps, *myTime))
        std::cout << "\nGlobal solar irradiance (clear sky) computed." << std::endl;
    else
        std::cout << "Error in compute radiation." << std::endl << std::endl;

    //SAVE OUTPUT
    *fileName = std::string("./data/globalRadiation");
    if (gis::writeEsriGrid(*fileName, radMaps->globalRadiationMap, error))
        std::cout << "Map saved in: " << *fileName << std::endl;
    else
        std::cout << "Error in writing:" << *fileName << std::endl << *error << std::endl;

    return a.exec();
}


