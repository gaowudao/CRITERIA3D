#ifndef ELABORATIONSETTINGS_H
#define ELABORATIONSETTINGS_H

#define DEF_VALUE_MIN_PERCENTAGE 80
#define DEF_VALUE_RAINFALL_THRESHOLD 0.2
#define DEF_VALUE_ANOMALY_PTS_MAX_DISTANCE 3000
#define DEF_VALUE_ANOMALY_PTS_MAX_DELTA_Z 50
#define DEF_VALUE_THOM_THRESHOLD 24
#define DEF_VALUE_GRID_MIN_COVERAGE 0
#define DEF_VALUE_TRANSMISSIVITY_SAMANI 0.17
#define DEF_VALUE_AUTOMATIC_T_MED true
#define DEF_VALUE_AUTOMATIC_ETP true
#define DEF_VALUE_MERGE_JOINT_STATIONS true


class Crit3DElaborationSettings
{
public:
    Crit3DElaborationSettings();
    float getMinimumPercentage() const;
    void setMinimumPercentage(float value);

    float getRainfallThreshold() const;
    void setRainfallThreshold(float value);

    float getAnomalyPtsMaxDistance() const;
    void setAnomalyPtsMaxDistance(float value);

    float getAnomalyPtsMaxDeltaZ() const;
    void setAnomalyPtsMaxDeltaZ(float value);

    float getThomThreshold() const;
    void setThomThreshold(float value);

    float getGridMinCoverage() const;
    void setGridMinCoverage(float value);

    float getTransSamaniCoefficient() const;
    void setTransSamaniCoefficient(float value);

    bool getAutomaticTmed() const;
    void setAutomaticTmed(bool value);

    bool getAutomaticETP() const;
    void setAutomaticETP(bool value);

    bool getMergeJointStations() const;
    void setMergeJointStations(bool value);

private:
    float minimumPercentage;
    float rainfallThreshold;
    float anomalyPtsMaxDistance;
    float anomalyPtsMaxDeltaZ;
    float thomThreshold;
    float gridMinCoverage;
    float transSamaniCoefficient;
    bool automaticTmed;
    bool automaticETP;
    bool mergeJointStations;

};

#endif // ELABORATIONSETTINGS_H
