#include "soil3D.h"

Crit3DSoilFluxesSettings::Crit3DSoilFluxesSettings()
{
    nrSoils = 0;
    nrSoilLayers = 0;
    nrLayerNodes = 0;
    nrNodes = 0;
    nrLateralLink = 8;
    soilDepth = 1.0;

    soilList = nullptr;
}

Crit3DSoilFluxesMaps::Crit3DSoilFluxesMaps()
{

}
