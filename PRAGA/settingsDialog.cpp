#include "settingsDialog.h"
#include "commonConstants.h"


GeoTab::GeoTab(gis::Crit3DGisSettings *gisSettings)
{
    QLabel *startLocationLat = new QLabel(tr("<b>start location latitude </b> (negative for Southern Emisphere) [decimal degrees]:"));
    QDoubleValidator *doubleValLat = new QDoubleValidator( -90.0, 90.0, 5, this );
    doubleValLat->setNotation(QDoubleValidator::StandardNotation);
    startLocationLatEdit.setFixedWidth(130);
    startLocationLatEdit.setValidator(doubleValLat);
    startLocationLatEdit.setText(QString::number(gisSettings->startLocation.latitude));


    QLabel *startLocationLon = new QLabel(tr("<b>start location longitude </b> [decimal degrees]:"));
    QDoubleValidator *doubleValLon = new QDoubleValidator( -180.0, 180.0, 5, this );
    doubleValLon->setNotation(QDoubleValidator::StandardNotation);
    startLocationLonEdit.setFixedWidth(130);
    startLocationLonEdit.setValidator(doubleValLon);
    startLocationLonEdit.setText(QString::number(gisSettings->startLocation.longitude));

    QLabel *utmZone = new QLabel(tr("UTM zone:"));
    utmZoneEdit.setFixedWidth(130);
    utmZoneEdit.setValidator(new QIntValidator(0, 60));
    utmZoneEdit.setText(QString::number(gisSettings->utmZone));

    QLabel *timeConvention = new QLabel(tr("Time Convention:"));
    QButtonGroup *group = new QButtonGroup();
    group->setExclusive(true);
    utc.setText("UTC");
    localTime.setText("Local Time");
    if (gisSettings->isUTC)
    {
        utc.setChecked(true);
        localTime.setChecked(false);
    }
    else
    {
        utc.setChecked(false);
        localTime.setChecked(true);
    }
    group->addButton(&utc);
    group->addButton(&localTime);
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(startLocationLat);
    mainLayout->addWidget(&startLocationLatEdit);

    mainLayout->addWidget(startLocationLon);
    mainLayout->addWidget(&startLocationLonEdit);

    mainLayout->addWidget(utmZone);
    mainLayout->addWidget(&utmZoneEdit);

    buttonLayout->addWidget(timeConvention);
    buttonLayout->addWidget(&utc);
    buttonLayout->addWidget(&localTime);

    mainLayout->addLayout(buttonLayout);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QualityTab::QualityTab(Crit3DQuality *quality)
{
    QLabel *referenceClimateHeight = new QLabel(tr("reference height for quality control [m]:"));
    QDoubleValidator *doubleValHeight = new QDoubleValidator( -100.0, 100.0, 5, this );
    doubleValHeight->setNotation(QDoubleValidator::StandardNotation);
    referenceClimateHeightEdit.setFixedWidth(130);
    referenceClimateHeightEdit.setValidator(doubleValHeight);
    referenceClimateHeightEdit.setText(QString::number(quality->getReferenceHeight()));



    QLabel *deltaTSuspect = new QLabel(tr("difference in temperature in climatological control (suspect value) [degC]:"));
    QDoubleValidator *doubleValT = new QDoubleValidator( -100.0, 100.0, 5, this );
    doubleValT->setNotation(QDoubleValidator::StandardNotation);
    deltaTSuspectEdit.setFixedWidth(130);
    deltaTSuspectEdit.setValidator(doubleValT);
    deltaTSuspectEdit.setText(QString::number(quality->getDeltaTSuspect()));


    QLabel *deltaTWrong = new QLabel(tr("difference in temperature in climatological control (wrong value) [degC]:"));
    deltaTWrongEdit.setFixedWidth(130);
    deltaTWrongEdit.setValidator(doubleValT);
    deltaTWrongEdit.setText(QString::number(quality->getDeltaTWrong()));

    QLabel *humidityTolerance = new QLabel(tr("instrumental maximum allowed relative humidity [%]:"));
    humidityToleranceEdit.setFixedWidth(130);
    QDoubleValidator *doubleValPerc = new QDoubleValidator( 0.0, 100.0, 5, this );
    doubleValPerc->setNotation(QDoubleValidator::StandardNotation);
    humidityToleranceEdit.setFixedWidth(130);
    humidityToleranceEdit.setValidator(doubleValPerc);
    humidityToleranceEdit.setText(QString::number(quality->getRelHumTolerance()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(referenceClimateHeight);
    mainLayout->addWidget(&referenceClimateHeightEdit);

    mainLayout->addWidget(deltaTSuspect);
    mainLayout->addWidget(&deltaTSuspectEdit);

    mainLayout->addWidget(deltaTWrong);
    mainLayout->addWidget(&deltaTWrongEdit);

    mainLayout->addWidget(humidityTolerance);
    mainLayout->addWidget(&humidityToleranceEdit);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

ElaborationTab::ElaborationTab(Crit3DElaborationSettings *elabSettings)
{
    QLabel *minimumPercentage = new QLabel(tr("minimum percentage of valid data [%]:"));
    minimumPercentageEdit.setFixedWidth(130);
    QDoubleValidator *doubleValPerc = new QDoubleValidator( 0.0, 100.0, 5, this );
    doubleValPerc->setNotation(QDoubleValidator::StandardNotation);
    minimumPercentageEdit.setFixedWidth(130);
    minimumPercentageEdit.setValidator(doubleValPerc);
    minimumPercentageEdit.setText(QString::number(elabSettings->getMinimumPercentage()));

    QLabel *rainfallThreshold = new QLabel(tr("minimum value for valid precipitation [mm]:"));
    QDoubleValidator *doubleValThreshold = new QDoubleValidator( 0.0, 20.0, 5, this );
    doubleValThreshold->setNotation(QDoubleValidator::StandardNotation);
    rainfallThresholdEdit.setFixedWidth(130);
    rainfallThresholdEdit.setValidator(doubleValThreshold);
    rainfallThresholdEdit.setText(QString::number(elabSettings->getRainfallThreshold()));

    QLabel *anomalyPtsMaxDis = new QLabel(tr("maximum distance between points for anomaly [m]:"));
    QDoubleValidator *doubleValAnomalyDis = new QDoubleValidator( -100.0, 100.0, 5, this );
    doubleValAnomalyDis->setNotation(QDoubleValidator::StandardNotation);
    anomalyPtsMaxDisEdit.setFixedWidth(130);
    anomalyPtsMaxDisEdit.setValidator(doubleValAnomalyDis);
    anomalyPtsMaxDisEdit.setText(QString::number(elabSettings->getAnomalyPtsMaxDistance()));

    QLabel *anomalyPtsMaxDeltaZ = new QLabel(tr("maximum height difference between points for anomaly [m]:"));
    QDoubleValidator *doubleValAnomalyDelta = new QDoubleValidator( -100.0, 100.0, 5, this );
    doubleValAnomalyDelta->setNotation(QDoubleValidator::StandardNotation);
    anomalyPtsMaxDeltaZEdit.setFixedWidth(130);
    anomalyPtsMaxDeltaZEdit.setValidator(doubleValAnomalyDelta);
    anomalyPtsMaxDeltaZEdit.setText(QString::number(elabSettings->getAnomalyPtsMaxDeltaZ()));

    QLabel *thomThreshold = new QLabel(tr("threshold for thom index [degC]:"));
    QDoubleValidator *doubleValThom = new QDoubleValidator( -100.0, 100.0, 5, this );
    doubleValThom->setNotation(QDoubleValidator::StandardNotation);
    thomThresholdEdit.setFixedWidth(130);
    thomThresholdEdit.setValidator(doubleValThom);
    thomThresholdEdit.setText(QString::number(elabSettings->getThomThreshold()));

    QLabel *gridMinCoverage = new QLabel(tr("minimum coverage for grid computation [%]:"));
    gridMinCoverageEdit.setFixedWidth(130);
    gridMinCoverageEdit.setValidator(doubleValPerc);
    gridMinCoverageEdit.setText(QString::number(elabSettings->getGridMinCoverage()));

    QLabel *transSamaniCoefficient = new QLabel(tr("Samani coefficient for ET0 computation []:"));
    QDoubleValidator *doubleValSamani = new QDoubleValidator( -5.0, 5.0, 5, this );
    doubleValSamani->setNotation(QDoubleValidator::StandardNotation);
    transSamaniCoefficientEdit.setFixedWidth(130);
    transSamaniCoefficientEdit.setValidator(doubleValSamani);
    transSamaniCoefficientEdit.setText(QString::number(elabSettings->getTransSamaniCoefficient()));

    QHBoxLayout *TmedLayout = new QHBoxLayout;
    QLabel *automaticTmed = new QLabel(tr("compute daily tmed from tmin and tmax when missing:"));
    TmedLayout->addWidget(automaticTmed);
    automaticTmedEdit.setChecked(elabSettings->getAutomaticTmed());
    TmedLayout->addWidget(&automaticTmedEdit);

    QHBoxLayout *ETPLayout = new QHBoxLayout;
    QLabel *automaticETP = new QLabel(tr("compute Hargreaves-Samani ET0 when missing:"));
    ETPLayout->addWidget(automaticETP);
    automaticETPEdit.setChecked(elabSettings->getAutomaticETP());
    ETPLayout->addWidget(&automaticETPEdit);

    QHBoxLayout *StationsLayout = new QHBoxLayout;
    QLabel *mergeJointStations = new QLabel(tr("automatically merge joint stations:"));
    StationsLayout->addWidget(mergeJointStations);
    mergeJointStationsEdit.setChecked(elabSettings->getMergeJointStations());
    StationsLayout->addWidget(&mergeJointStationsEdit);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(minimumPercentage);
    mainLayout->addWidget(&minimumPercentageEdit);

    mainLayout->addWidget(rainfallThreshold);
    mainLayout->addWidget(&rainfallThresholdEdit);

    mainLayout->addWidget(anomalyPtsMaxDis);
    mainLayout->addWidget(&anomalyPtsMaxDisEdit);

    mainLayout->addWidget(anomalyPtsMaxDeltaZ);
    mainLayout->addWidget(&anomalyPtsMaxDeltaZEdit);

    mainLayout->addWidget(thomThreshold);
    mainLayout->addWidget(&thomThresholdEdit);

    mainLayout->addWidget(gridMinCoverage);
    mainLayout->addWidget(&gridMinCoverageEdit);

    mainLayout->addWidget(transSamaniCoefficient);
    mainLayout->addWidget(&transSamaniCoefficientEdit);

    mainLayout->addLayout(TmedLayout);

    mainLayout->addLayout(ETPLayout);

    mainLayout->addLayout(StationsLayout);


    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

SettingsDialog::SettingsDialog(QSettings *pathSetting, QSettings *settings, gis::Crit3DGisSettings *gisSettings, Crit3DQuality *quality, Crit3DElaborationSettings *elabSettings)
{

    _pathSettings = pathSetting;
    _paramSettings = settings;

    _geoSettings = gisSettings;
    _qualitySettings = quality;
    _elabSettings = elabSettings;

    setWindowTitle(tr("Parameters"));
    setFixedSize(650,700);
    geoTab = new GeoTab(gisSettings);
    qualityTab = new QualityTab(quality);
    elabTab = new ElaborationTab(elabSettings);

    tabWidget = new QTabWidget;
    tabWidget->addTab(geoTab, tr("GEO"));
    tabWidget->addTab(qualityTab, tr("QUALITY"));
    tabWidget->addTab(elabTab, tr("ELABORATION"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    exec();

}


void SettingsDialog::accept()
{

        if (geoTab->startLocationLatEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert start location latitude");
            return;
        }

        if (geoTab->startLocationLonEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert start location longitude");
            return;
        }

        if (geoTab->utmZoneEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert UTM zone");
            return;
        }

        if (!geoTab->utc.isChecked() && !geoTab->localTime.isChecked())
        {
            QMessageBox::information(NULL, "Missing time convention", "choose UTC or local time");
            return;
        }

        ////////////////

        if (qualityTab->referenceClimateHeightEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert reference height for quality control");
            return;
        }

        if (qualityTab->deltaTSuspectEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert difference in temperature suspect value");
            return;
        }

        if (qualityTab->deltaTWrongEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert difference in temperature wrong value");
            return;
        }

        if (qualityTab->humidityToleranceEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "instrumental maximum allowed relative humidity");
            return;
        }

        ////////////////////

        if (elabTab->minimumPercentageEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert minimum percentage of valid data");
            return;
        }

        if (elabTab->rainfallThresholdEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert minimum value for precipitation");
            return;
        }

        if (elabTab->anomalyPtsMaxDisEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert maximum distance between points");
            return;
        }

        if (elabTab->anomalyPtsMaxDeltaZEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert maximum height difference between points");
            return;
        }

        if (elabTab->thomThresholdEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert threshold for thom index");
            return;
        }

        if (elabTab->gridMinCoverageEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert minimum coverage for grid computation");
            return;
        }

        if (elabTab->transSamaniCoefficientEdit.text().isEmpty())
        {
            QMessageBox::information(NULL, "Missing Parameter", "insert Samani coefficient for ET0 computation");
            return;
        }

        // store elaboration values

        _geoSettings->startLocation.latitude = geoTab->startLocationLatEdit.text().toDouble();
        _geoSettings->startLocation.longitude = geoTab->startLocationLonEdit.text().toDouble();
        _geoSettings->utmZone = geoTab->utmZoneEdit.text().toInt();
        _geoSettings->isUTC = geoTab->utc.isChecked();
        if (_geoSettings->startLocation.latitude >= 0)
        {
            _geoSettings->isNorthernEmisphere = true;
        }
        else
        {
            _geoSettings->isNorthernEmisphere = false;
        }

        _qualitySettings->setReferenceHeight(qualityTab->referenceClimateHeightEdit.text().toFloat());
        _qualitySettings->setDeltaTSuspect(qualityTab->deltaTSuspectEdit.text().toFloat());
        _qualitySettings->setDeltaTWrong(qualityTab->deltaTWrongEdit.text().toFloat());
        _qualitySettings->setRelHumTolerance(qualityTab->humidityToleranceEdit.text().toFloat());

        _elabSettings->setMinimumPercentage(elabTab->minimumPercentageEdit.text().toFloat());
        _elabSettings->setRainfallThreshold(elabTab->rainfallThresholdEdit.text().toFloat());
        _elabSettings->setAnomalyPtsMaxDistance(elabTab->anomalyPtsMaxDisEdit.text().toFloat());
        _elabSettings->setAnomalyPtsMaxDeltaZ(elabTab->anomalyPtsMaxDeltaZEdit.text().toFloat());
        _elabSettings->setThomThreshold(elabTab->thomThresholdEdit.text().toFloat());
        _elabSettings->setGridMinCoverage(elabTab->gridMinCoverageEdit.text().toFloat());
        _elabSettings->setTransSamaniCoefficient(elabTab->transSamaniCoefficientEdit.text().toFloat());

        _elabSettings->setAutomaticTmed(elabTab->automaticTmedEdit.isChecked());
        _elabSettings->setAutomaticETP(elabTab->automaticETPEdit.isChecked());
        _elabSettings->setMergeJointStations(elabTab->mergeJointStationsEdit.isChecked());

        //saveSettings();

        QDialog::done(QDialog::Accepted);
        return;

}

void SettingsDialog::saveSettings()
{

    _pathSettings->beginGroup("location");
    _pathSettings->setValue("lat", geoTab->startLocationLatEdit.text());
    _pathSettings->setValue("lon", geoTab->startLocationLonEdit.text());
    _pathSettings->setValue("utm_zone", geoTab->utmZoneEdit.text());
    _pathSettings->setValue("is_utc", geoTab->utc.isChecked());
    _pathSettings->endGroup();

    _paramSettings->beginGroup("quality");
    _paramSettings->setValue("reference_height", qualityTab->referenceClimateHeightEdit.text());
    _paramSettings->setValue("delta_temperature_suspect", qualityTab->deltaTSuspectEdit.text());
    _paramSettings->setValue("delta_temperature_wrong", qualityTab->deltaTWrongEdit.text());
    _paramSettings->setValue("relhum_tolerance", qualityTab->humidityToleranceEdit.text());
    _paramSettings->endGroup();

    _paramSettings->beginGroup("elaboration");
    _paramSettings->setValue("min_percentage", elabTab->minimumPercentageEdit.text());
    _paramSettings->setValue("prec_threshold", elabTab->rainfallThresholdEdit.text());
    _paramSettings->setValue("anomaly_pts_max_distance", elabTab->anomalyPtsMaxDisEdit.text());
    _paramSettings->setValue("anomaly_pts_max_delta_z", elabTab->anomalyPtsMaxDeltaZEdit.text());
    _paramSettings->setValue("thom_threshold", elabTab->thomThresholdEdit.text());
    _paramSettings->setValue("grid_min_coverage", elabTab->gridMinCoverageEdit.text());
    _paramSettings->setValue("samani_coefficient", elabTab->transSamaniCoefficientEdit.text());
    _paramSettings->setValue("compute_tmed", elabTab->automaticTmedEdit.isChecked());
    _paramSettings->setValue("compute_et0hs", elabTab->automaticETPEdit.isChecked());
    _paramSettings->setValue("merge_joint_stations", elabTab->mergeJointStationsEdit.isChecked());
    _paramSettings->endGroup();

}
