#ifndef CRITERIA3D
#define CRITERIA3D

    #ifdef BUILD_DLL
        #define DLL_EXPORT __declspec(dllexport)
        #define __EXTERN extern "C"
        #define __STDCALL __stdcall
    #else
        #define DLL_EXPORT
        #define __EXTERN
        #define __STDCALL
    #endif

    namespace soilFluxes3D {

    //INITIALIZATION
    __EXTERN void DLL_EXPORT __STDCALL cleanMemory();
    __EXTERN int DLL_EXPORT __STDCALL initialize(long nrNodes, int nrLayers, int nrLateralLinks);

    __EXTERN int DLL_EXPORT __STDCALL setNumericalParameters(float minDeltaT, float maxDeltaT,
                              int maxIterationNumber, int maxApproximationsNumber,
                              int errorMagnitude, float MBRMagnitude);

    //TOPOLOGY
    __EXTERN int DLL_EXPORT __STDCALL setNode(long myIndex, float x, float y, float z, double volume_or_area,
                                        bool isSurface, bool isBoundary, int boundaryType, float slope);

    __EXTERN int DLL_EXPORT __STDCALL setNodeLink(long nodeIndex, long linkIndex, int direction, float S0);

    //SOIL
    __EXTERN int DLL_EXPORT __STDCALL setSoilProperties(int nrSoil, int nrHorizon, double VG_alpha,
                                        double VG_n, double VG_m, double VG_he,
                                        double ThetaR, double ThetaS, double Ksat, double L);

    __EXTERN int DLL_EXPORT __STDCALL setNodeSoil(long nodeIndex, int soilIndex, int horizonIndex);

    //SURFACE
    __EXTERN int DLL_EXPORT __STDCALL setSurfaceProperties(int surfaceIndex, double Roughness, double minWaterLevelRunoff);

    __EXTERN int DLL_EXPORT __STDCALL setNodeSurface(long nodeIndex, int surfaceIndex);

    //WATER
    __EXTERN int DLL_EXPORT __STDCALL setHydraulicProperties(int waterRetentionCurve,
                                                   int conductivityMeanType, float horizVertRatioConductivity);
    __EXTERN int DLL_EXPORT __STDCALL setWaterContent(long index, double myWaterContent);
    __EXTERN int DLL_EXPORT __STDCALL setMatricPotential(long index, double potential);
    __EXTERN int DLL_EXPORT __STDCALL setTotalPotential(long index, double totalPotential);
    __EXTERN int DLL_EXPORT __STDCALL setPrescribedTotalPotential(long index, double prescribedTotalPotential);
    __EXTERN int DLL_EXPORT __STDCALL setWaterSinkSource(long index, double sinkSource);

    __EXTERN double DLL_EXPORT __STDCALL getWaterContent(long index);
    __EXTERN double DLL_EXPORT __STDCALL getAvailableWaterContent(long index);
    __EXTERN double DLL_EXPORT __STDCALL getWaterDeficit(long index, double fieldCapacity);
    __EXTERN double DLL_EXPORT __STDCALL getTotalWaterContent();
    __EXTERN double DLL_EXPORT __STDCALL getDegreeOfSaturation(long index);
    __EXTERN double DLL_EXPORT __STDCALL getBoundaryWaterFlow(long index);
    __EXTERN double DLL_EXPORT __STDCALL getBoundaryWaterSumFlow(int boundaryType);
    __EXTERN double DLL_EXPORT __STDCALL getMatricPotential(long index);
    __EXTERN double DLL_EXPORT __STDCALL getTotalPotential(long index);
    __EXTERN double DLL_EXPORT __STDCALL getWaterMBR();
    __EXTERN double DLL_EXPORT __STDCALL getWaterConductivity(long index);
    __EXTERN double DLL_EXPORT __STDCALL getWaterFlow(long index, short direction);
    __EXTERN double DLL_EXPORT __STDCALL getSumLateralWaterFlow(long n);


    //HEAT


    //SOLUTES


    //COMPUTATION
    __EXTERN void DLL_EXPORT __STDCALL initializeBalance();
    __EXTERN void DLL_EXPORT __STDCALL computePeriod(float myPeriod);

}

#endif
