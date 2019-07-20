#include "project3D.h"


Project3D::Project3D() : Project()
{
    nrSoils = 0;
    soilDepth = 1.0;            // [m]

    minThickness = 0.02;        // [m]
    maxThickness = 0.1;         // [m]
    thickFactor = 1.5;

    nrLayers = 0;
    nrNodes = 0;
    nrNodesPerLayer = 0;
    nrLateralLink = 8;

    meteoMaps = nullptr;
}


int Project3D::computeNrLayers(double totalDepth)
 {
    int nrLayers = 1;
    double nextThickness, prevThickness = minThickness;
    double depth = minThickness * 0.5;

    while (depth < totalDepth)
    {
        nextThickness = minValue(maxThickness, prevThickness * thickFactor);
        depth = depth + (prevThickness + nextThickness) * 0.5;
        prevThickness = nextThickness;
        nrLayers++;
    }

    return nrLayers;
}


// set thickness and depth (center) of layers [m]
bool Project3D::setLayersDepth()
{
    int lastLayer = nrLayers-1;
    layerDepth.resize(unsigned(nrLayers));
    layerThickness.resize(unsigned(nrLayers));

    layerDepth[0] = 0.0;
    layerThickness[0] = 0.0;
    layerThickness[1] = minThickness;
    layerDepth[1] = minThickness * 0.5;
    for (unsigned int i = 2; i < unsigned(nrLayers); i++)
    {
        if (i == unsigned(lastLayer))
        {
            layerThickness[i] = soilDepth - (layerDepth[i-1]
                                          + layerThickness[i-1] / 2.0);
        }
        else
        {
            layerThickness[i] = minValue(maxThickness, layerThickness[i-1] * thickFactor);
        }
        layerDepth[i] = layerDepth[i-1] +
                                (layerThickness[i-1] + layerThickness[i]) * 0.5f;
    }

    return true;
}
