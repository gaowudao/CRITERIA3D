#include "project3D.h"


Project3D::Project3D() : Project()
{
    soilDepth = 1.0;
    nrSoils = 0;
    nrLayers = 0;
    nrNodes = 0;
    nrNodesPerLayer = 0;
    nrLateralLink = 8;

    meteoMaps = nullptr;
}
