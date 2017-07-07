#define PROGRESSION_ARITHMETIC 0
#define PROGRESSION_GEOMETRIC 1

#include <qpoint.h>
#include <qvector.h>
#include <qdatetime.h>
#include "graphs.h"

struct Qsoil{
    double profSup;
    double profInf;
    double VG_alfa;			// [m^-1] alfa Van Genutchen
    double VG_n;			// [-] n Van Genutchen
    double VG_m;			// [-] n Van Genutchen ]0. , 1.[
    double VG_he;			// [m] air-entry potential [0 , 1]
    double VG_Sc;			// [-] reduction factor for modified VG formulation
    double Theta_s;			// [m^3/m^3] theta saturo
    double Theta_r;			// [m^3/m^3] theta residuo
    double K_sat;			// [m/sec] conducibilit� idraulica satura
    double Mualem_L;		// [-] parametro tortuosita' della formula conducibilit� di Mualem
    double Clay;			// [-] normalized Clay content [0,1]
    double OrganicMatter;	// [-] normalized Organic matter content [0,1)
    double CEC;				// [meq hg^-1] Cation Excange Capacity [0,150]
    double PH;				// [-] PH [3,10]

    double Roughness;		// [s/m^0.33] superficie: scabrezza superficiale (parametro alfa eq. di Manning)
    double Plough;			// [m] superficie: altezza acqua minima(pozzanghera) prima che inizi il deflusso superficiale
    } ;


extern Qsoil *myInputSoils;

void setDimension(short myValue);
void setProgression(short myProgressionType);
void setSurface(double myArea, double myAlbedo, double myRoughness, double minWaterRunoff, double myRoughnessHeat);
void setSoil(double thetaS_, double thetaR_, double clay_, double organicMatter_);
void setTotalDepth(double myValue);
void setMinThickness(double myValue);
void setMaxThickness(double myValue);
void setSimulationStart(int myValue);
void setSimulationStop(int myValue);
void setInitialSaturation(double myValue);
void setInitialTemperature(double myValueTop, double myValueBottom);
void setBottomTemperature(double myValue);
void setHeatProcesses(bool computeHeatAdvection, bool computeHeatLatent, bool computeHeatSensible);
void setProcesses(bool computeWaterProcess, bool computeHeatProcess, bool computeSolutesProcess);
void setHour(long myHour);
double getCurrentPrec(long myHour);
void getHourlyOutput(long myHour, long firstIndex, long lastIndex, QString& myString);
void getHourlyOutputAllPeriod(long firstIndex, long lastIndex, heat_output *output);
long getNodesNumber();
void setSoilHorizonNumber(int myHorizonNumber);

bool initializeTestHeat(long *myHourIni, long *myHourFin, bool useInputSoils);
bool runTestHeat(double myHourlyTemperature,  double myHourlyRelativeHumidity,
                 double myHourlyWindSpeed, double myHourlyNetIrradiance,
                 double myHourlyPrec);


