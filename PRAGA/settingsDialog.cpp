#include <QtWidgets>
#include "settingsDialog.h"


GeoTab::GeoTab()
{
    QLabel *startLocationLat = new QLabel(tr("start location latitude [decimal degrees]:"));
    QLineEdit *startLocationLatEdit = new QLineEdit();

    QLabel *startLocationLon = new QLabel(tr("start location longitude [decimal degrees]:"));
    QLineEdit *startLocationLonEdit = new QLineEdit();

    QLabel *utmZone = new QLabel(tr("UTM zone:"));
    QLineEdit *utmZoneEdit = new QLineEdit();

    QLabel *northernHemisphere = new QLabel(tr("true (false): Northern (Southern) emisphere:"));
    QLineEdit *northernHemisphereEdit = new QLineEdit();

    QLabel *dataTimeType = new QLabel(tr("true (false): UTC (local time):"));
    QLineEdit *dataTimeTypeEdit = new QLineEdit();


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(startLocationLat);
    mainLayout->addWidget(startLocationLatEdit);

    mainLayout->addWidget(startLocationLon);
    mainLayout->addWidget(startLocationLonEdit);

    mainLayout->addWidget(utmZone);
    mainLayout->addWidget(utmZoneEdit);

    mainLayout->addWidget(northernHemisphere);
    mainLayout->addWidget(northernHemisphereEdit);

    mainLayout->addWidget(dataTimeType);
    mainLayout->addWidget(dataTimeTypeEdit);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QualityTab::QualityTab()
{
    QLabel *referenceClimateHeight = new QLabel(tr("reference height for quality control [m]:"));
    QLineEdit *sreferenceClimateHeightEdit = new QLineEdit();

    QLabel *deltaTSuspect = new QLabel(tr("difference in temperature in climatological control (suspect value) [degC]:"));
    QLineEdit *deltaTSuspectEdit = new QLineEdit();

    QLabel *deltaTWrong = new QLabel(tr("difference in temperature in climatological control (wrong value) [degC]:"));
    QLineEdit *deltaTWrongEdit = new QLineEdit();

    QLabel *humidityTolerance = new QLabel(tr("instrumental maximum allowed relative humidity [%]:"));
    QLineEdit *humidityToleranceEdit = new QLineEdit();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(referenceClimateHeight);
    mainLayout->addWidget(sreferenceClimateHeightEdit);

    mainLayout->addWidget(deltaTSuspect);
    mainLayout->addWidget(deltaTSuspectEdit);

    mainLayout->addWidget(deltaTWrong);
    mainLayout->addWidget(deltaTWrongEdit);

    mainLayout->addWidget(humidityTolerance);
    mainLayout->addWidget(humidityToleranceEdit);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

ElaborationTab::ElaborationTab()
{
    QLabel *minimumPercentage = new QLabel(tr("minimum percentage of valid data [%]:"));
    QLineEdit *minimumPercentageEdit = new QLineEdit();

    QLabel *rainfallThreshold = new QLabel(tr("minimum value for valid precipitation [mm]:"));
    QLineEdit *rainfallThresholdEdit = new QLineEdit();

    QLabel *anomalyPtsMaxDis = new QLabel(tr("maximum distance between points for anomaly [m]:"));
    QLineEdit *anomalyPtsMaxDisEdit = new QLineEdit();

    QLabel *anomalyPtsMaxDeltaZ = new QLabel(tr("maximum height difference between points for anomaly [m]:"));
    QLineEdit *anomalyPtsMaxDeltaZEdit = new QLineEdit();

    QLabel *thomThreshold = new QLabel(tr("threshold for thom index [degC]:"));
    QLineEdit *thomThresholdEdit = new QLineEdit();

    QLabel *automaticTmed = new QLabel(tr("compute daily tmed from tmin and tmax when missing:"));
    QLineEdit *automaticTmedEdit = new QLineEdit();

    QLabel *automaticETP = new QLabel(tr("compute Hargreaves-Samani ET0 when missing:"));
    QLineEdit *automaticETPEdit = new QLineEdit();

    QLabel *gridMinCoverage = new QLabel(tr("minimum coverage for grid computation [%]:"));
    QLineEdit *gridMinCoverageEdit = new QLineEdit();

    QLabel *transSamaniCoefficient = new QLabel(tr("Samani coefficient for ET0 computation []:"));
    QLineEdit *transSamaniCoefficientEdit = new QLineEdit();

    QLabel *mergeJointStations = new QLabel(tr("automatically merge joint stations:"));
    QLineEdit *mergeJointStationsEdit = new QLineEdit();


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(minimumPercentage);
    mainLayout->addWidget(minimumPercentageEdit);

    mainLayout->addWidget(rainfallThreshold);
    mainLayout->addWidget(rainfallThresholdEdit);

    mainLayout->addWidget(anomalyPtsMaxDis);
    mainLayout->addWidget(anomalyPtsMaxDisEdit);

    mainLayout->addWidget(anomalyPtsMaxDeltaZ);
    mainLayout->addWidget(anomalyPtsMaxDeltaZEdit);

    mainLayout->addWidget(thomThreshold);
    mainLayout->addWidget(thomThresholdEdit);

    mainLayout->addWidget(automaticTmed);
    mainLayout->addWidget(automaticTmedEdit);

    mainLayout->addWidget(automaticETP);
    mainLayout->addWidget(automaticETPEdit);

    mainLayout->addWidget(gridMinCoverage);
    mainLayout->addWidget(gridMinCoverageEdit);

    mainLayout->addWidget(transSamaniCoefficient);
    mainLayout->addWidget(transSamaniCoefficientEdit);

    mainLayout->addWidget(mergeJointStations);
    mainLayout->addWidget(mergeJointStationsEdit);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

SettingsDialog::SettingsDialog()
{

    setWindowTitle(tr("Parameters"));
    setFixedSize(650,700);
    geoTab = new GeoTab();
    qualityTab = new QualityTab();
    elabTab = new ElaborationTab();

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

    show();

}
