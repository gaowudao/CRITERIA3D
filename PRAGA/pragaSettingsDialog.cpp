#include "pragaSettingsDialog.h"

ElaborationTab::ElaborationTab(Crit3DElaborationSettings *elabSettings)
{

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

    QLabel *gridMinCoverage = new QLabel(tr("minimum coverage for grid computation [%]:"));
    QDoubleValidator *doubleValPerc = new QDoubleValidator( 0.0, 100.0, 5, this );
    gridMinCoverageEdit.setFixedWidth(130);
    gridMinCoverageEdit.setValidator(doubleValPerc);
    gridMinCoverageEdit.setText(QString::number(elabSettings->getGridMinCoverage()));

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
    mainLayout->addWidget(anomalyPtsMaxDis);
    mainLayout->addWidget(&anomalyPtsMaxDisEdit);

    mainLayout->addWidget(anomalyPtsMaxDeltaZ);
    mainLayout->addWidget(&anomalyPtsMaxDeltaZEdit);

    mainLayout->addWidget(gridMinCoverage);
    mainLayout->addWidget(&gridMinCoverageEdit);

    mainLayout->addLayout(TmedLayout);

    mainLayout->addLayout(ETPLayout);

    mainLayout->addLayout(StationsLayout);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}


PragaSettingsDialog::PragaSettingsDialog(QSettings *projectSettings,
                                         QSettings *settings,
                                         gis::Crit3DGisSettings *gisSettings,
                                         Crit3DQuality *quality,
                                         Crit3DMeteoSettings *meteoSettings,
                                         Crit3DElaborationSettings *elabSettings) : DialogSettings(projectSettings, settings, gisSettings, quality, meteoSettings)
{
    _elabSettings = elabSettings;
    elabTab = new ElaborationTab(elabSettings);

    getTabWidget()->addTab(elabTab, tr("ELABORATION"));
}

bool PragaSettingsDialog::acceptPragaValues()
{
    if (elabTab->anomalyPtsMaxDisEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert maximum distance between points");
        return false;
    }

    if (elabTab->anomalyPtsMaxDeltaZEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert maximum height difference between points");
        return false;
    }

    if (elabTab->gridMinCoverageEdit.text().isEmpty())
    {
        QMessageBox::information(nullptr, "Missing Parameter", "insert minimum coverage for grid computation");
        return false;
    }

    // store elaboration values

    _elabSettings->setGridMinCoverage(elabTab->gridMinCoverageEdit.text().toFloat());
    _elabSettings->setAnomalyPtsMaxDistance(elabTab->anomalyPtsMaxDisEdit.text().toFloat());
    _elabSettings->setAnomalyPtsMaxDeltaZ(elabTab->anomalyPtsMaxDeltaZEdit.text().toFloat());
    _elabSettings->setAutomaticTmed(elabTab->automaticTmedEdit.isChecked());
    _elabSettings->setAutomaticETP(elabTab->automaticETPEdit.isChecked());
    _elabSettings->setMergeJointStations(elabTab->mergeJointStationsEdit.isChecked());

    savePragaSettings();

    return true;
}

void PragaSettingsDialog::accept()
{
    if (acceptValues() && acceptPragaValues())
    {
        QDialog::done(QDialog::Accepted);
        return;
    }
}

void PragaSettingsDialog::savePragaSettings()
{
    getParamSettings()->beginGroup("elaboration");
    getParamSettings()->setValue("anomaly_pts_max_distance", elabTab->anomalyPtsMaxDisEdit.text());
    getParamSettings()->setValue("anomaly_pts_max_delta_z", elabTab->anomalyPtsMaxDeltaZEdit.text());
    getParamSettings()->setValue("grid_min_coverage", elabTab->gridMinCoverageEdit.text());
    getParamSettings()->setValue("compute_tmed", elabTab->automaticTmedEdit.isChecked());
    getParamSettings()->setValue("compute_et0hs", elabTab->automaticETPEdit.isChecked());
    getParamSettings()->setValue("merge_joint_stations", elabTab->mergeJointStationsEdit.isChecked());
    getParamSettings()->endGroup();
}
