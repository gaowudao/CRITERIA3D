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

MeteoTab::MeteoTab(Crit3DMeteoSettings *meteoSettings)
{
    QLabel *minimumPercentage = new QLabel(tr("minimum percentage of valid data [%]:"));
    minimumPercentageEdit.setFixedWidth(130);
    QDoubleValidator *doubleValPerc = new QDoubleValidator( 0.0, 100.0, 5, this );
    doubleValPerc->setNotation(QDoubleValidator::StandardNotation);
    minimumPercentageEdit.setFixedWidth(130);
    minimumPercentageEdit.setValidator(doubleValPerc);
    minimumPercentageEdit.setText(QString::number(meteoSettings->getMinimumPercentage()));

    QLabel *rainfallThreshold = new QLabel(tr("minimum value for valid precipitation [mm]:"));
    QDoubleValidator *doubleValThreshold = new QDoubleValidator( 0.0, 20.0, 5, this );
    doubleValThreshold->setNotation(QDoubleValidator::StandardNotation);
    rainfallThresholdEdit.setFixedWidth(130);
    rainfallThresholdEdit.setValidator(doubleValThreshold);
    rainfallThresholdEdit.setText(QString::number(meteoSettings->getRainfallThreshold()));

    QLabel *thomThreshold = new QLabel(tr("threshold for thom index [degC]:"));
    QDoubleValidator *doubleValThom = new QDoubleValidator( -100.0, 100.0, 5, this );
    doubleValThom->setNotation(QDoubleValidator::StandardNotation);
    thomThresholdEdit.setFixedWidth(130);
    thomThresholdEdit.setValidator(doubleValThom);
    thomThresholdEdit.setText(QString::number(meteoSettings->getThomThreshold()));

    QLabel *transSamaniCoefficient = new QLabel(tr("Samani coefficient for ET0 computation []:"));
    QDoubleValidator *doubleValSamani = new QDoubleValidator( -5.0, 5.0, 5, this );
    doubleValSamani->setNotation(QDoubleValidator::StandardNotation);
    transSamaniCoefficientEdit.setFixedWidth(130);
    transSamaniCoefficientEdit.setValidator(doubleValSamani);
    transSamaniCoefficientEdit.setText(QString::number(meteoSettings->getTransSamaniCoefficient()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(minimumPercentage);
    mainLayout->addWidget(&minimumPercentageEdit);

    mainLayout->addWidget(rainfallThreshold);
    mainLayout->addWidget(&rainfallThresholdEdit);

    mainLayout->addWidget(thomThreshold);
    mainLayout->addWidget(&thomThresholdEdit);

    mainLayout->addWidget(transSamaniCoefficient);
    mainLayout->addWidget(&transSamaniCoefficientEdit);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}


SettingsDialog::SettingsDialog(QSettings *projectSettings, QSettings *settings, gis::Crit3DGisSettings *gisSettings, Crit3DQuality *quality, Crit3DMeteoSettings *meteoSettings)
{
    _pathSettings = projectSettings;
    _paramSettings = settings;

    _geoSettings = gisSettings;
    _qualitySettings = quality;
    _meteoSettings = meteoSettings;

    setWindowTitle(tr("Parameters"));
    setFixedSize(650,700);
    geoTab = new GeoTab(gisSettings);
    qualityTab = new QualityTab(quality);
    metTab = new MeteoTab(meteoSettings);

    tabWidget = new QTabWidget;
    tabWidget->addTab(geoTab, tr("GEO"));
    tabWidget->addTab(qualityTab, tr("QUALITY"));
    tabWidget->addTab(metTab, tr("METEO"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

bool SettingsDialog::acceptValues()
{

    if (geoTab->startLocationLatEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert start location latitude");
        return false;
    }

    if (geoTab->startLocationLonEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert start location longitude");
        return false;
    }

    if (geoTab->utmZoneEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert UTM zone");
        return false;
    }

    if (!geoTab->utc.isChecked() && !geoTab->localTime.isChecked())
    {
        QMessageBox::information(nullptr, "Missing time convention", "choose UTC or local time");
        return false;
    }

    ////////////////

    if (qualityTab->referenceClimateHeightEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert reference height for quality control");
        return false;
    }

    if (qualityTab->deltaTSuspectEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert difference in temperature suspect value");
        return false;
    }

    if (qualityTab->deltaTWrongEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert difference in temperature wrong value");
        return false;
    }

    if (qualityTab->humidityToleranceEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "instrumental maximum allowed relative humidity");
        return false;
    }

    ////////////////////

    if (metTab->minimumPercentageEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert minimum percentage of valid data");
        return false;
    }

    if (metTab->rainfallThresholdEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert minimum value for precipitation");
        return false;
    }

    if (metTab->thomThresholdEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert threshold for thom index");
        return false;
    }

    if (metTab->transSamaniCoefficientEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert Samani coefficient for ET0 computation");
        return false;
    }

    // store elaboration values

    _geoSettings->startLocation.latitude = geoTab->startLocationLatEdit.text().toDouble();
    _geoSettings->startLocation.longitude = geoTab->startLocationLonEdit.text().toDouble();
    _geoSettings->utmZone = geoTab->utmZoneEdit.text().toInt();
    _geoSettings->isUTC = geoTab->utc.isChecked();

    _qualitySettings->setReferenceHeight(qualityTab->referenceClimateHeightEdit.text().toFloat());
    _qualitySettings->setDeltaTSuspect(qualityTab->deltaTSuspectEdit.text().toFloat());
    _qualitySettings->setDeltaTWrong(qualityTab->deltaTWrongEdit.text().toFloat());
    _qualitySettings->setRelHumTolerance(qualityTab->humidityToleranceEdit.text().toFloat());

    _meteoSettings->setMinimumPercentage(metTab->minimumPercentageEdit.text().toFloat());
    _meteoSettings->setRainfallThreshold(metTab->rainfallThresholdEdit.text().toFloat());
    _meteoSettings->setThomThreshold(metTab->thomThresholdEdit.text().toFloat());
    _meteoSettings->setTransSamaniCoefficient(metTab->transSamaniCoefficientEdit.text().toFloat());

    saveSettings();

    return true;
}

void SettingsDialog::accept()
{
    if (acceptValues())
    {
        QDialog::done(QDialog::Accepted);
        return;
    }
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

    _paramSettings->beginGroup("meteo");
    _paramSettings->setValue("min_percentage", metTab->minimumPercentageEdit.text());
    _paramSettings->setValue("prec_threshold", metTab->rainfallThresholdEdit.text());
    _paramSettings->setValue("samani_coefficient", metTab->transSamaniCoefficientEdit.text());
    _paramSettings->setValue("thom_threshold", metTab->thomThresholdEdit.text());
    _paramSettings->endGroup();
}

QTabWidget *SettingsDialog::getTabWidget() const
{
    return tabWidget;
}

void SettingsDialog::setTabWidget(QTabWidget *value)
{
    tabWidget = value;
}

QSettings *SettingsDialog::getParamSettings() const
{
    return _paramSettings;
}

void SettingsDialog::setParamSettings(QSettings *paramSettings)
{
    _paramSettings = paramSettings;
}
