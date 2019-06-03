#include "unitCropMap.h"
#include "zonalStatistic.h"

// TO DO

// function unitCropMap(6 campi)
// 1) copiare crop > UCM (memoria se si può fare)
// 2) zonalstatistic UCM MeteoGrid (id_meteo)
// 3) zonalstatistic UCM Soil (id_soil)
// 4) add UCM agli shape in interfaccia

bool unitCropMap(Crit3DShapeHandler *ucm, Crit3DShapeHandler *crop, Crit3DShapeHandler *soil, Crit3DShapeHandler *meteo, std::string idSoil, std::string idMeteo, int cellSize, std::string *error)
{

    /*
    *ucm = *crop;
    bool ucmSoil = zonalStatisticsShape(ucm, soil, idSoil, cellSize, MAJORITY, error);
    if (!ucmSoil)
    {
        *error = "zonalStatisticsShape ucm soil Error";
        return false;
    }

    bool ucmMeteo = zonalStatisticsShape(ucm, meteo, idMeteo, cellSize, MAJORITY, error);
    if (!ucmMeteo)
    {
        *error = "zonalStatisticsShape ucm meteo Error";
        return false;
    }
    return true;
    */

}
