#include <QtWidgets>

#include "project.h"
#include "radiationSettings.h"
#include "dialogRadiation.h"

DialogRadiation::DialogRadiation(Project* myProject)
{
    project_ = myProject;

    setWindowTitle(tr("Radiation settings"));
    QVBoxLayout layoutMain;

    // algorithm
    QHBoxLayout layoutAlgorithm;

    QLabel labelAlgorithm("algorithm");
    layoutAlgorithm.addWidget(&labelAlgorithm);

    std::map<std::string, TradiationAlgorithm>::const_iterator itAggr;
    for (itAggr = radAlgorithmToString.begin(); itAggr != radAlgorithmToString.end(); ++itAggr)
        comboAlgorithm.addItem(QString::fromStdString(itAggr->first), QString::fromStdString(itAggr->first));

    QString radString = QString::fromStdString(getKeyStringRadAlgorithm(project_->radSettings.getAlgorithm()));
    int indexRad = comboAlgorithm.findData(radString);
    if (indexRad != -1)
       comboAlgorithm.setCurrentIndex(indexRad);

    layoutAlgorithm.addWidget(&comboAlgorithm);

    layoutMain.addLayout(&layoutAlgorithm);

    // transmissivity settings
    QHBoxLayout layoutTransmissivity;

    checkRealSky.setText("Real sky");
    checkRealSky.setChecked(project_->radSettings.getComputeRealData());
    layoutTransmissivity.addWidget(&checkRealSky);

/*

    QCheckBox checkRealSky;
    QComboBox comboTransmissAlgorithm;
    QComboBox comboTransmissPeriod;
    QCheckBox checkUseTotalTransmiss;
    QLineEdit editTransClearSky;

    QComboBox comboLinkeMode;
    QLineEdit editLinke;
    QLineEdit editLinkeMap;

    QComboBox comboAlbedoMode;
    QLineEdit editAlbedo;
    QLineEdit editAlbedoMap;

    QComboBox tiltMode;
    QLineEdit editTilt;
    QLineEdit editTiltMap;
    QLineEdit editAspect;

    QCheckBox checkShadowing;
    */
}

void DialogRadiation::accept()
{

}
