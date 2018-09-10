#include "elaborationSettings.h"

Crit3DElaborationSettings::Crit3DElaborationSettings()
{
    minimumPercentage = DEF_VALUE_MIN_PERCENTAGE;
    rainfallThreshold = DEF_VALUE_RAINFALL_THRESHOLD;
    anomalyPtsMaxDistance = DEF_VALUE_ANOMALY_PTS_MAX_DISTANCE;
    anomalyPtsMaxDeltaZ = DEF_VALUE_ANOMALY_PTS_MAX_DELTA_Z;
    thomThreshold = DEF_VALUE_THOM_THRESHOLD;
    gridMinCoverage = DEF_VALUE_GRID_MIN_COVERAGE;
    transSamaniCoefficient = DEF_VALUE_TRANSMISSIVITY_SAMANI;
    automaticTmed = DEF_VALUE_AUTOMATIC_T_MED;
    automaticETP = DEF_VALUE_AUTOMATIC_ETP;
    mergeJointStations = DEF_VALUE_MERGE_JOINT_STATIONS;
}

float Crit3DElaborationSettings::getMinimumPercentage() const
{
    return minimumPercentage;
}

void Crit3DElaborationSettings::setMinimumPercentage(float value)
{
    minimumPercentage = value;
}

float Crit3DElaborationSettings::getRainfallThreshold() const
{
    return rainfallThreshold;
}

void Crit3DElaborationSettings::setRainfallThreshold(float value)
{
    rainfallThreshold = value;
}

float Crit3DElaborationSettings::getAnomalyPtsMaxDistance() const
{
    return anomalyPtsMaxDistance;
}

void Crit3DElaborationSettings::setAnomalyPtsMaxDistance(float value)
{
    anomalyPtsMaxDistance = value;
}

float Crit3DElaborationSettings::getAnomalyPtsMaxDeltaZ() const
{
    return anomalyPtsMaxDeltaZ;
}

void Crit3DElaborationSettings::setAnomalyPtsMaxDeltaZ(float value)
{
    anomalyPtsMaxDeltaZ = value;
}

float Crit3DElaborationSettings::getThomThreshold() const
{
    return thomThreshold;
}

void Crit3DElaborationSettings::setThomThreshold(float value)
{
    thomThreshold = value;
}

float Crit3DElaborationSettings::getGridMinCoverage() const
{
    return gridMinCoverage;
}

void Crit3DElaborationSettings::setGridMinCoverage(float value)
{
    gridMinCoverage = value;
}

float Crit3DElaborationSettings::getTransSamaniCoefficient() const
{
    return transSamaniCoefficient;
}

void Crit3DElaborationSettings::setTransSamaniCoefficient(float value)
{
    transSamaniCoefficient = value;
}

bool Crit3DElaborationSettings::getAutomaticTmed() const
{
    return automaticTmed;
}

void Crit3DElaborationSettings::setAutomaticTmed(bool value)
{
    automaticTmed = value;
}

bool Crit3DElaborationSettings::getAutomaticETP() const
{
    return automaticETP;
}

void Crit3DElaborationSettings::setAutomaticETP(bool value)
{
    automaticETP = value;
}

bool Crit3DElaborationSettings::getMergeJointStations() const
{
    return mergeJointStations;
}

void Crit3DElaborationSettings::setMergeJointStations(bool value)
{
    mergeJointStations = value;
}

