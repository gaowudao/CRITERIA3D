#ifndef SOILFLUXES3DSETTINGS_H
#define SOILFLUXES3DSETTINGS_H

#ifndef VECTOR_H
    #include <vector>
#endif

#ifndef GIS_H
    #include "gis.h"
#endif

#ifndef SOIL_H
    #include "soil.h"
#endif

class Crit3DSoilFluxesSettings
{
    public:
        Crit3DSoilFluxesSettings();

        double minThinkness;
        double maxThickness;
        double thickFactor;
        int nrLateralLink;

        int nrSoils;
        int nrSoilLayers;
        float soilDepth;
        soil::Crit3DSoil* soilList;

        long nrNodes;
        long nrLayerNodes;
        double* currentProfile;

        std::vector <double> layerDepth;
        std::vector <double> layerThickness;
};

class Crit3DSoilFluxesMaps
{
    public:
        Crit3DSoilFluxesMaps();

        std::vector <gis::Crit3DRasterGrid> indexMap;
};

#endif // SOILFLUXES3DSETTINGS_H
