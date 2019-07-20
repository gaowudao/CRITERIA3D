#ifndef PROJECT3D_H
#define PROJECT3D_H

    #ifndef PROJECT_H
        #include "project.h"
    #endif
    #ifndef METEOMAPS_H
        #include "meteoMaps.h"
    #endif
    #ifndef SOIL_H
        #include "soil.h"
    #endif


    class Project3D : public Project
    {

    public:
        Crit3DMeteoMaps* meteoMaps;

        gis::Crit3DRasterGrid soilMap;
        std::vector <gis::Crit3DRasterGrid> indexMap;

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


        Project3D();


        int computeNrLayers(double totalDepth);
        bool setLayersDepth();
    };


#endif // PROJECT3D_H
