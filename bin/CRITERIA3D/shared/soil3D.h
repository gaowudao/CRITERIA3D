#ifndef SOIL3D_H
#define SOIL3D_H

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

            double minThickness;
            double maxThickness;
            double thickFactor;
            int nrLateralLink;

            int nrSoils;
            int nrLayers;
            double soilDepth;                       //[m]

            std::vector <soil::Crit3DSoil> soilList;
            soil::Crit3DTextureClass texturalClassList[13];

            long nrNodes;
            long nrNodesPerLayer;

            std::vector <double> layerDepth;        //[m]
            std::vector <double> layerThickness;    //[m]
    };

    class Crit3DSoilFluxesMaps
    {
        public:
            Crit3DSoilFluxesMaps();

            std::vector <gis::Crit3DRasterGrid> indexMap;
    };

#endif // SOIL3D_H
