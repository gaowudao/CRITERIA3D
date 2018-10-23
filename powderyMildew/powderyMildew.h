#ifndef MILDEW_H
#define MILDEW_H

#include <vector>

#ifndef NODATA
    #define NODATA -9999
#endif

// constants
const double delta = 0.969;
const double lambda = 0.0004;
const double fi = 7.391;
const double nu = 2.403;
const double csi = 0.892;
const double upsilon = 0.221;
const double gamma = 44.7;
const double psi = 0.067;
const double theta = 3.244;

struct TmildewInput
{
    float tavg;
    float rain;
    int leafWetness;
    float relativeHumidity;
};

struct TmildewOutput
{
    bool dayInfection;
    bool daySporulation;
    float infectionRate;
    float infectionRisk;
    float aol;
    float col;
};


struct TmildewState
{
    float degreeDays;
    float aic;                              // Ascospores in Chasmothecia
    float currentColonies;                  // total Colony-forming ascospores
    float totalSporulatingColonies;
};

struct Tmildew
{
    TmildewInput input;
    TmildewOutput output;
    TmildewState state;
};

void powderyMildew(Tmildew* mildewCore, bool isFirst);
float computeDegreeDay(float temp);
float ascosporesReadyFraction(float degreeDay);
float ascosporeDischargeRate(float temp, float rain, int leafWetness);
float vpd(float temp,float relativeHumidity);
float infectionRate(float temp, float vapourPressure);
float latencyProgress(float temp);
float max_vector(const std::vector<float>& v);


#endif // MILDEW_H
