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
    QLabel labelTrans("Real sky settings");
    layoutTransmissivity.addWidget(&labelTrans);
    QVBoxLayout layoutTransSettings;

    checkRealSky.setText("Real sky");
    checkRealSky.setChecked(project_->radSettings.getComputeRealData());
    layoutTransSettings.addWidget(&checkRealSky);

    checkUseTotalTransmiss.setText("use total transmissivity");
    checkUseTotalTransmiss.setChecked(project_->radSettings.getTransmissivityUseTotal());
    layoutTransSettings.addWidget(&checkUseTotalTransmiss);

    QLabel labelTransClear("Clear sky transmissivity");
    editTransClearSky.setText(QString::number(project_->radSettings.getClearSky()));
    QDoubleValidator *doubleVal = new QDoubleValidator(0.0, 1.0, 2, this);
    editTransClearSky.setValidator(doubleVal);
    layoutTransSettings.addWidget(&labelTransClear);
    layoutTransSettings.addWidget(&editTransClearSky);
    layoutTransmissivity.addLayout(&layoutTransSettings);

    layoutMain.addLayout(&layoutTransmissivity);
/*

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

    layoutMain.addStretch(1);
    setLayout(&layoutMain);

    exec();
}

void DialogRadiation::accept()
{

}
