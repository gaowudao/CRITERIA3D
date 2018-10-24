#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QtWidgets>

#include "gis.h"
#include "quality.h"
#include "meteo.h"
#include "elaborationSettings.h"

class QDialogButtonBox;
class QFileInfo;
class QTabWidget;


class GeoTab : public QWidget
{
    Q_OBJECT

public:
    explicit GeoTab(gis::Crit3DGisSettings *gisSettings);

    QLineEdit startLocationLatEdit;
    QLineEdit startLocationLonEdit;
    QLineEdit utmZoneEdit;
    QCheckBox utc;
    QCheckBox localTime ;


private:

};

class QualityTab : public QWidget
{
    Q_OBJECT

public:
    explicit QualityTab(Crit3DQuality *quality);

    QLineEdit referenceClimateHeightEdit;
    QLineEdit deltaTSuspectEdit;
    QLineEdit deltaTWrongEdit;
    QLineEdit humidityToleranceEdit;

private:

};

class ElaborationTab : public QWidget
{
    Q_OBJECT

public:
    explicit ElaborationTab(Crit3DElaborationSettings *elabSettings);

    QLineEdit anomalyPtsMaxDisEdit;
    QLineEdit anomalyPtsMaxDeltaZEdit;
    QLineEdit gridMinCoverageEdit;
    QCheckBox automaticTmedEdit;
    QCheckBox automaticETPEdit;
    QCheckBox mergeJointStationsEdit;

private:
};

class MeteoTab : public QWidget
{
    Q_OBJECT

public:
    explicit MeteoTab(Crit3DMeteoSettings *meteoSettings);

    QLineEdit minimumPercentageEdit;
    QLineEdit rainfallThresholdEdit;
    QLineEdit thomThresholdEdit;
    QLineEdit transSamaniCoefficientEdit;

private:
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit SettingsDialog(QSettings *pathSetting, QSettings *settings, gis::Crit3DGisSettings *gisSettings, Crit3DQuality *quality, Crit3DElaborationSettings *elabSettings, Crit3DMeteoSettings* meteoSettings);
        void accept();
        void saveSettings();

    private:
        QSettings *_pathSettings;
        QSettings *_paramSettings;
        gis::Crit3DGisSettings *_geoSettings;
        Crit3DQuality *_qualitySettings;
        Crit3DElaborationSettings *_elabSettings;
        Crit3DMeteoSettings* _meteoSettings;

        QTabWidget *tabWidget;
        QDialogButtonBox *buttonBox;
        GeoTab* geoTab;
        QualityTab* qualityTab;
        ElaborationTab* elabTab;
        MeteoTab* metTab;
};

#endif // SETTINGSDIALOG_H
