/*!
    \copyright 2016 Fausto Tomei, Gabriele Antolini,
    Alberto Pistocchi, Marco Bittelli, Antonio Volta, Laura Costantini

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by ARPAE Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/

/*! TestSolarRadiation
 * compute a map of global solar irradiance (clear sky)
 * for a specified date/time, starting from a DTM input.
*/

#include <QCoreApplication>
#include "iostream"
#include "commonConstants.h"
#include "gis.h"
#include "radiationSettings.h"
#include "solarRadiation.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // DTM INPUT
    std::string path = a.applicationDirPath().toStdString() + "/";
    std::string fileName = path + ("../../DATA/DEM/dem_ravone");

    // GIS SETTINGS (UTM zone, Time zone)
    gis::Crit3DGisSettings* gisSettings = new gis::Crit3DGisSettings();
    gisSettings->utmZone = 32;
    gisSettings->timeZone = 1;

    // DATETIME (UTC time)
    Crit3DDate* myDate = new Crit3DDate(1,6, 2018);
    int myHour = 11;

    std::cout << "\nTEST Solar Radiation library" << std::endl;
    std::cout << "Compute a map of global solar irradiance (clear sky)" << std::endl;
    std::cout << "UTM zone: " << gisSettings->utmZone << std::endl;
    std::cout << "Date: " << myDate->toStdString() << " hour: " << myHour << " UTC" << std::endl;

    // READ DTM
    gis::Crit3DRasterGrid* dtm = new gis::Crit3DRasterGrid();
    std::string* error = new std::string();
    if (gis::readEsriGrid(fileName, dtm, error))
        std::cout << "\nDTM loaded: " << fileName << std::endl;
    else
    {
        std::cout << "Error in reading:" << fileName << std::endl << *error << std::endl;
        return 0;
    }

    // SET RADIATION SETTINGS
    Crit3DRadiationSettings* radSettings = new Crit3DRadiationSettings();
    radSettings->setGisSettings(gisSettings);

    // INITIALIZE RADIATION MAPS (deafult trasmissivity = 0.75)
    Crit3DRadiationMaps* radMaps = new Crit3DRadiationMaps(*dtm, *gisSettings);

    float mySeconds = float(HOUR_SECONDS * myHour);
    Crit3DTime* myTime = new Crit3DTime(*myDate, mySeconds);

    std::cout << "\nComputing..." << std::endl;

    // COMPUTE POTENTIAL GLOBAL RADIATION MAPS
    if (radiation::computeRadiationGridPresentTime(radSettings, *dtm, radMaps, *myTime))
        std::cout << "\nGlobal solar irradiance (clear sky) computed." << std::endl;
    else
        std::cout << "Error in compute radiation." << std::endl << std::endl;

    //SAVE OUTPUT
    fileName = path + std::string("globalRadiation");
    if (gis::writeEsriGrid(fileName, radMaps->globalRadiationMap, error))
        std::cout << "Map saved in: " << fileName << std::endl;
    else
        std::cout << "Error in writing:" << fileName << std::endl << *error << std::endl;

    return a.exec();
}


