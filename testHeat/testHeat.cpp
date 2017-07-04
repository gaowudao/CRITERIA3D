// testHeat.cpp : main project file.

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <qglobal.h>
#include <qlist.h>

#include "commonConstants.h"
#include "testHeat.h"
#include "soilFluxes3D.h"
#include "types.h"
#include "boundary.h"
#include "soilPhysics.h"

heat_output output;

//structure
short myDimension;
short verticalNodeProgression;
long NodesNumber;
double myTotalDepth ;
double myMinThickness, myMaxThickness;
long simulationStart, simulationStop;

//surface
double mySurface, Roughness, Plough, Albedo, RoughnessHeat;

//bottom boundary
double bottomTemperature;

//meteo
double airRelativeHumidity, airTemperature, windSpeed, globalRad;
double precHourlyAmount, precHours, precIniHour, precTemperature;

//initialization
double initialSaturation, initialTemperatureTop, initialTemperatureBottom;

//processes
bool computeWater, computeSolutes, computeHeat;

long CurrentHour;

//soils
int myHorizonNumber;
double ThetaS, ThetaR, Clay, OrganicMatter;

void setDimension(short myValue)
{   myDimension = myValue;}

void setProgression(short myValue)
{   verticalNodeProgression = myValue;}

void setTotalDepth(double myValue)
{   myTotalDepth = myValue;}

void setMinThickness(double myValue)
{   myMinThickness = myValue;}

void setMaxThickness(double myValue)
{   myMaxThickness = myValue;}

void setSimulationStart(int myValue)
{   simulationStart = myValue;}

void setSimulationStop(int myValue)
{   simulationStop = myValue;}

void setInitialSaturation(double myValue)
{   initialSaturation = myValue;}

void setInitialTemperature(double myValueTop, double myValueBottom)
{   initialTemperatureTop = myValueTop;
    initialTemperatureBottom = myValueBottom;
}

void setBottomTemperature(double myValue)
{
    bottomTemperature = myValue;
}

void setSoil(double thetaS_, double thetaR_, double clay_, double organicMatter_)
{
    ThetaS = thetaS_;
    ThetaR = thetaR_;
    Clay = clay_;
    OrganicMatter = organicMatter_;
}

void setProcesses(bool computeWaterProcess, bool computeHeat_, bool computeSolutesProcess)
{
    computeWater = computeWaterProcess;
    computeHeat = computeHeat_;
    computeSolutes = computeSolutesProcess;
}

void getHourlyOutput(long myHour, long firstIndex, long lastIndex, QString& myString)
{
    double myWaterContent, h, myTemperature;
    double myDiffusiveHeat, myIsoLatHeat;
	long myIndex;
    double WaterSum = 0;

    myString.append("wc(m3m-3),psi(m),T(degC),diffHeatFlux(Wm-2),isoLatHeat(Wm-2) HOUR: "); // WV (1000*kg m-3) HOUR: ");
    myString.append(QString::number(myHour,'f',0));
    myString.append(QString("\n"));

    for (myIndex = firstIndex ; myIndex <= lastIndex ; myIndex++ )
		{ 
        myWaterContent = soilFluxes3D::getWaterContent(myIndex);
        h = soilFluxes3D::getMatricPotential(myIndex);
        myTemperature = soilFluxes3D::getTemperature(myIndex);
        myDiffusiveHeat = soilFluxes3D::getHeatFlux(myIndex, DOWN, HEATFLUX_DIFFUSIVE);
        myIsoLatHeat = soilFluxes3D::getHeatFlux(myIndex, DOWN, HEATFLUX_LATENT_ISOTHERMAL);

        myString.append(QString::number(myWaterContent,'f',12));
        myString.append(QString(", "));
        myString.append(QString::number(h,'f',6));
        myString.append(QString(", "));

        if (myTemperature != INDEX_ERROR)
            myString.append(QString::number(myTemperature - 273.16,'f',6));
        else
            myString.append(QString::number(myTemperature,'f',6));

        myString.append(QString(", "));
        myString.append(QString::number(myDiffusiveHeat,'f',6));
        myString.append(QString(", "));
        myString.append(QString::number(myIsoLatHeat,'f',6));
        myString.append(QString(", "));

        WaterSum += myWaterContent * myNode[myIndex].volume_area;

        myString.append(QString("\n"));
		}

    myString.append(QString("WaterSum:"));
    myString.append(QString::number(WaterSum,'f',9));
    myString.append(QString(" m3 MBRWater:"));
    myString.append(QString::number(soilFluxes3D::getWaterMBR(),'f',9));
    myString.append(QString(" MBEHeat:"));
    myString.append(QString::number(balanceCurrentTimeStep.heatMBE,'f',9));
    myString.append(QString(" MJ MBRHeat:"));
    myString.append(QString::number(balanceCurrentTimeStep.heatMBR,'f',9));
    myString.append(QString("\n"));
    myString.append(QString("\n"));
}


void emptyOutput()
{
    output.errorOutput.clear();
    output.landSurfaceOutput.clear();
    output.profileOutput.clear();
}

heat_output::heat_output()
{
    nrValues = 0;
    nrLayers = 0;

    this->errorOutput.clear();
    this->landSurfaceOutput.clear();
    this->profileOutput.clear();
}
void getHourlyOutputAllPeriod(long firstIndex, long lastIndex, QString& myString, heat_output* output)
{
    long myIndex;
    double myValue;
    QPointF myPoint;
    profileStatus myProfile;
    landSurfaceStatus mySurfaceOutput;
    heatErrors myErrors;

    output->nrValues++;
    output->profileOutput.append(myProfile);
    output->landSurfaceOutput.append(mySurfaceOutput);
    output->errorOutput.append(myErrors);

    for (myIndex = firstIndex ; myIndex <= lastIndex ; myIndex++ )
    {
        myPoint.setX(myIndex);

        myValue = soilFluxes3D::getTemperature(myIndex) - 273.16;
        myPoint.setY(myValue);
        output->profileOutput[output->nrValues-1].temperature.append(myPoint);
        myString.append(QString::number(myValue,'f',4));
        myString.append(QString(", "));

        myValue = soilFluxes3D::getWaterContent(myIndex);
        myPoint.setY(myValue);
        output->profileOutput[output->nrValues-1].waterContent.append(myPoint);
        myString.append(QString::number(myValue,'f',4));
        myString.append(QString(", "));

        myValue = soilFluxes3D::getHeatFlux(myIndex, UP, HEATFLUX_DIFFUSIVE);
        myValue += soilFluxes3D::getHeatFlux(myIndex, UP, HEATFLUX_LATENT_ISOTHERMAL);
        myValue += soilFluxes3D::getHeatFlux(myIndex, UP, HEATFLUX_LATENT_THERMAL);
        myValue += soilFluxes3D::getHeatFlux(myIndex, UP, HEATFLUX_ADVECTIVE);
        myPoint.setY(myValue);
        output->profileOutput[output->nrValues-1].totalHeatFlux.append(myPoint);
        myString.append(QString::number(myValue,'f',4));
        myString.append(QString(", "));

        myValue = soilFluxes3D::getHeatFlux(myIndex, UP, HEATFLUX_LATENT_ISOTHERMAL);
        myPoint.setY(myValue);
        output->profileOutput[output->nrValues-1].isothermalLatentHeatFlux.append(myPoint);
        myString.append(QString::number(myValue,'f',4));
        myString.append(QString(", "));

        myValue = soilFluxes3D::getHeatFlux(myIndex, UP, HEATFLUX_LATENT_THERMAL);
        myPoint.setY(myValue);
        output->profileOutput[output->nrValues-1].thermalLatentHeatFlux.append(myPoint);

        myValue = soilFluxes3D::getHeatFlux(myIndex, UP, HEATFLUX_DIFFUSIVE);
        myPoint.setY(myValue);
        output->profileOutput[output->nrValues-1].diffusiveHeatFlux.append(myPoint);

        myValue = soilFluxes3D::getHeatFlux(myIndex, UP, HEATFLUX_ADVECTIVE);
        myPoint.setY(myValue);
        output->profileOutput[output->nrValues-1].advectiveheatFlux.append(myPoint);
    }

    myPoint.setX(output->landSurfaceOutput.size() + 1);

    // net radiation (positive downward)
    myValue = soilFluxes3D::getBoundaryRadiativeFlux(1);
    myPoint.setY(myValue);
    output->landSurfaceOutput[output->nrValues-1].netRadiation = myPoint;
    myString.append(QString::number(myValue,'f',2));
    myString.append(QString(", "));

    // sensible heat (positive upward)
    myValue = soilFluxes3D::getBoundarySensibleFlux(1);
    myPoint.setY(myValue);
    output->landSurfaceOutput[output->nrValues-1].sensibleHeat = myPoint;
    myString.append(QString::number(myValue,'f',2));
    myString.append(QString(", "));

    myString.append(QString::number(soilFluxes3D::getBoundaryAdvectiveFlux(1),'f',2));
    myString.append(QString(", "));

    // latent heat (positive upward)
    myValue = soilFluxes3D::getBoundaryLatentFlux(1);
    myPoint.setY(myValue);
    output->landSurfaceOutput[output->nrValues-1].latentHeat = myPoint;
    myString.append(QString::number(myValue,'f',2));
    myString.append(QString(", "));

    //evaporation
    myValue = -myValue * 1000 * 3600 * mySurface;
    myString.append(QString::number(myValue,'f',6));
    myString.append(QString(", "));

    //aerodynamic resistance
    myValue = soilFluxes3D::getBoundaryAerodynamicConductance(1);
    myPoint.setY(myValue);
    output->landSurfaceOutput[output->nrValues-1].aeroConductance = myPoint;
    myString.append(QString::number(1./myValue,'f',6));
    myString.append(QString(", "));

    //soil surface resistance
    myValue = soilFluxes3D::getBoundarySoilConductance(1);
    myPoint.setY(myValue);
    output->landSurfaceOutput[output->nrValues-1].soilConductance = myPoint;
    myString.append(QString::number(1./myValue,'f',6));
    myString.append(QString(", "));

    //errors
    myValue = soilFluxes3D::GetHeatMBR();
    myPoint.setY(myValue);
    output->errorOutput[output->nrValues-1].heatMBR = myPoint;
    myString.append(QString::number(myValue,'f',12));
    myString.append(QString(", "));

    myValue = soilFluxes3D::GetHeatMBE();
    myPoint.setY(myValue);
    output->errorOutput[output->nrValues-1].heatMBE = myPoint;
    myString.append(QString::number(myValue,'f',12));
    myString.append(QString(", "));

    myValue = soilFluxes3D::getWaterMBR();
    myPoint.setY(myValue);
    output->errorOutput[output->nrValues-1].waterMBR = myPoint;
    myString.append(QString::number(myValue,'f',12));
    myString.append(QString("\n"));

}

long getNodesNumber()
{   return (NodesNumber - 1);}

void setSoilHorizonNumber(int myNumber)
{   myHorizonNumber = myNumber;}

void setSurface(double myArea, double myAlbedo, double myRoughness, double minWaterRunoff, double myRoughnessHeat)
{
    mySurface = myArea;
    Albedo = myAlbedo;
    Roughness = myRoughness;
    Plough = minWaterRunoff;
    RoughnessHeat = myRoughnessHeat;

    int myResult = soilFluxes3D::setSurfaceProperties(0, Roughness, Plough);
    if (myResult != CRIT3D_OK) printf("\n error in SetSurfaceProperties!");
}


bool initializeSoil(bool useInputSoils)
{
    int MyResult;

    // loam (Troy soil db)
    double VG_he        = 0.023;    //m
    double VG_alpha     = 1.76;     //m-1
    double VG_n         = 1.21;
    double thetaR       = 0.03;
    double thetaS       = 0.42;
    double mualemTort   = 0.5;
    double KSat         = 1. / (3600. * 100.);

    if (useInputSoils)
    {
        for (int mySoilIndex = 0; mySoilIndex < myHorizonNumber; mySoilIndex++)
            MyResult = soilFluxes3D::setSoilProperties(0,mySoilIndex,
                                          myInputSoils[mySoilIndex].VG_alfa,
                                          myInputSoils[mySoilIndex].VG_n,
                                          myInputSoils[mySoilIndex].VG_m,
                                          myInputSoils[mySoilIndex].VG_he,
                                          myInputSoils[mySoilIndex].Theta_r,
                                          myInputSoils[mySoilIndex].Theta_s,
                                          myInputSoils[mySoilIndex].K_sat,
                                          myInputSoils[mySoilIndex].Mualem_L,
                                          myInputSoils[mySoilIndex].OrganicMatter,
                                          myInputSoils[mySoilIndex].Clay);
    }
    else
        MyResult = soilFluxes3D::setSoilProperties(0, 1, VG_alpha, VG_n, (1. - 1. / VG_n), VG_he, thetaR, thetaS, KSat, mualemTort, OrganicMatter/100., Clay/100.);

    if (MyResult != CRIT3D_OK) {
        printf("\n error in SetSoilProperties");
        return(false);
    }
    else return true;
}

bool initializeTestHeat(long *myHourIni, long *myHourFin, bool useInputSoils)
{
    int MyResult = 0;
    long FirstLayerNodesNumber;
    short int LayersNumber;
    long NodoIndice ;
    long Strato ;
    float x , y ; // z ;
    bool Impermeabile;
    bool CaricoImposto ;
    int SetNodoLink;
    double ThetaS = 0.42; // theta sat
    double ThetaR = 0.05;
    int boundaryType;

        if (myDimension == 1)
		{
            FirstLayerNodesNumber = 1;

            if (verticalNodeProgression == PROGRESSION_GEOMETRIC)
            {
            ; //TODO
            }
            else if (verticalNodeProgression == PROGRESSION_ARITHMETIC)
            {
            NodesNumber = ceil(myTotalDepth / myMinThickness) + 1;
		    LayersNumber = (short) NodesNumber;
            double *myDepth = (double *) calloc(LayersNumber, sizeof(double));
            double *Spessore = (double *) calloc(LayersNumber, sizeof(double));
			
			Spessore[0] = 0;
            myDepth[0] = 0.;

            int myNodeHorizon = 0;

            for (Strato = 1 ; Strato < LayersNumber ; Strato++ )
			{
                Spessore[Strato] = myMinThickness;
                if (Strato == 1) myDepth[Strato] = myDepth[Strato-1] - (myMinThickness / 2) ;
                else  myDepth[Strato] = myDepth[Strato-1] - myMinThickness ;
			}

            soilFluxes3D::initialize(NodesNumber, LayersNumber, 0, computeWater, computeHeat, computeSolutes);
            soilFluxes3D::initializeHeat(SAVE_HEATFLUXES_ALL);

            soilFluxes3D::setHydraulicProperties(MODIFIEDVANGENUCHTEN, MEAN_LOGARITHMIC, 10.);

            initializeSoil(useInputSoils);

			if (MyResult == CRIT3D_OK) printf("Topography Ok");
			x = y = 0.0 ; // inizializzazione di x e y
			
			CaricoImposto = Impermeabile = false ;

            for (NodoIndice = 0 ; NodoIndice < NodesNumber ; NodoIndice++ )
                {
				Strato = NodoIndice ; 
				
                // elemento superficiale
				if (NodoIndice == 0)
					{
                    MyResult = soilFluxes3D::setNode(NodoIndice, x, y, myDepth[Strato], mySurface, true, false, BOUNDARY_NONE, 0.0);
					if (MyResult != CRIT3D_OK) printf("\n error in SetNode!");

                    MyResult = soilFluxes3D::setNodeSurface(0, 0) ;
					if (MyResult != CRIT3D_OK) printf("\n error in SetNodeSurface!");

                    MyResult = soilFluxes3D::setWaterContent(NodoIndice, 0.);

                    MyResult = soilFluxes3D::SetTemperature(NodoIndice, 273.16 + initialTemperatureTop);
					}

                // elementi sottosuperficiali
                else
					{
                    if (NodoIndice == 1)
                    {
                        if (computeHeat)
                            boundaryType = BOUNDARY_HEAT;
                        else
                            boundaryType = BOUNDARY_NONE;

                        MyResult = soilFluxes3D::setNode(NodoIndice, x, y, myDepth[Strato], Spessore[Strato] * mySurface, false, true, boundaryType, 0.0);
                    }
                    else if (NodoIndice == NodesNumber - 1)
                    {
                        MyResult = soilFluxes3D::setNode(NodoIndice, x ,y, myDepth[Strato], Spessore[Strato] * mySurface, false, true, BOUNDARY_FREEDRAINAGE, 0.0);
                        MyResult = soilFluxes3D::SetFixedTemperature(NodoIndice, 273.16 + bottomTemperature);
                    }
                    else
                        MyResult = soilFluxes3D::setNode(NodoIndice, x, y, myDepth[Strato], Spessore[Strato] * mySurface, false, false, BOUNDARY_NONE, 0.0);
					
					if (MyResult != CRIT3D_OK) printf("\n error in SetNode!");

                    SetNodoLink = soilFluxes3D::setNodeLink(NodoIndice, NodoIndice - 1 , UP, mySurface);
					if (SetNodoLink != CRIT3D_OK) printf("\n error in SetNode int sopra!");
						
                    if (useInputSoils)
                    {if (myDepth[Strato]<myInputSoils[myNodeHorizon].profInf)
                            if (myNodeHorizon < myHorizonNumber-1)
                                myNodeHorizon++;}
                    else
                        myNodeHorizon = 1;

                    MyResult = soilFluxes3D::setNodeSoil(NodoIndice, 0, myNodeHorizon) ;

                    if (MyResult != CRIT3D_OK) printf("\n error in SetNodeSoil!");

                    if (initialSaturation <= 1. && initialSaturation > 0.)
                        if (useInputSoils)
                            MyResult = soilFluxes3D::setWaterContent(NodoIndice, initialSaturation * (myInputSoils[myNodeHorizon].Theta_s - myInputSoils[myNodeHorizon].Theta_r) + myInputSoils[myNodeHorizon].Theta_r);
                        else
                            MyResult = soilFluxes3D::setWaterContent(NodoIndice, initialSaturation * (ThetaS - ThetaR) + ThetaR);
                    else
                        printf("\n error in initial saturation degree!");

                    MyResult = soilFluxes3D::SetTemperature(NodoIndice,
                         273.16 + ((NodoIndice-1)*(initialTemperatureBottom-initialTemperatureTop)/(NodesNumber-2)+initialTemperatureTop));

                    if (MyResult != CRIT3D_OK) printf("\n error in SetTemperature!");

                    }

                    // tutti tranne ultimo nodo confinano con nodo sotto
                    if (NodoIndice < NodesNumber - 1)
                        {
                            SetNodoLink = soilFluxes3D::setNodeLink(NodoIndice, NodoIndice + 1 , DOWN, mySurface);
                            if (SetNodoLink != CRIT3D_OK) printf("\n error in SetNode int sotto!");
                        }

				if (MyResult != CRIT3D_OK) printf("\n error in SetTemperature!");
				
				}
			}
		}
        else
            printf("\n not supported");

    soilFluxes3D::setNumericalParameters(1, 600, 100, 10, 12, 5);

    soilFluxes3D::initializeBalance();

    *myHourIni = simulationStart;
    *myHourFin = simulationStop;

	return (true);
}

double getCurrentPrec(long myHour)
{ 
        if ((myHour >= precIniHour) && myHour < precIniHour + precHours)
                return (precHourlyAmount);
	else
		return (0.);
}

void setHour(long myHour)
{   CurrentHour = myHour;}

void setSinkSources(double myHourlyPrec)
{
    for (long i=0; i<NodesNumber; i++)
    {
        if (computeHeat) soilFluxes3D::SetHeatSinkSource(i, 0);

        if (computeWater)
        {
            if (i == 0)
                soilFluxes3D::setWaterSinkSource(i, (double)myHourlyPrec * mySurface / 3.6e06);
            else
                soilFluxes3D::setWaterSinkSource(i, 0);
        }
    }
}

bool runTestHeat(double myHourlyTemperature,  double myHourlyRelativeHumidity,
                 double myHourlyWindSpeed, double myHourlyNetIrradiance,
                 double myHourlyPrec)
{
    double currentRoughness;
    double surfaceWaterHeight;
    double roughnessWater = 0.005;

    setSinkSources(myHourlyPrec);

    if (computeHeat)
    {
        soilFluxes3D::SetHeatBoundaryHeightWind(1, 2);
        soilFluxes3D::SetHeatBoundaryHeightTemperature(1, 1.5);
        soilFluxes3D::SetHeatBoundaryTemperature(1, myHourlyTemperature);
        soilFluxes3D::SetHeatBoundaryRelativeHumidity(1, myHourlyRelativeHumidity);
        soilFluxes3D::SetHeatBoundaryWindSpeed(1, myHourlyWindSpeed);
        soilFluxes3D::SetHeatBoundaryNetIrradiance(1, myHourlyNetIrradiance);

        surfaceWaterHeight = soilFluxes3D::getWaterContent(0);
        if (surfaceWaterHeight > RoughnessHeat)
            currentRoughness = 0.005;
        else if (surfaceWaterHeight > 0.)
            currentRoughness = (roughnessWater - RoughnessHeat) / RoughnessHeat * surfaceWaterHeight + RoughnessHeat;
        else
            currentRoughness = RoughnessHeat;

        soilFluxes3D::SetHeatBoundaryRoughness(1, currentRoughness);
    }

    soilFluxes3D::computePeriod(HOUR_SECONDS);

	return (true);
}


