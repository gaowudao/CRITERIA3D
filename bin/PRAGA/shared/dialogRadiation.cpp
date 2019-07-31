#include <QtWidgets>

#include "radiationDefinitions.h"
#include "radiationSettings.h"
#include "dialogRadiation.h"

DialogRadiation::DialogRadiation(Project* myProject)
{
    project_ = myProject;

    setWindowTitle(tr("Radiation settings"));
    QVBoxLayout *layoutMain = new QVBoxLayout;

    /*
    QHBoxLayout *layoutTransmissivity = new QVBoxLayout();
    QLabel *labelAggregation = new QLabel(tr("aggregation method"));
    layoutAggregation->addWidget(labelAggregation);


    std::map<std::string, TradiationAlgorithm>::const_iterator itAggr;
    for (itAggr = radAlgorithmToString.begin(); itAggr != radAlgorithmToString.end(); ++itAggr)
        comboAlgorithm.addItem(QString::fromStdString(itAggr->first), QString::fromStdString(itAggr->first));

    QString radString = QString::fromStdString(getkeys(_interpolationSettings->getMeteoGridAggrMethod()));
    int indexAggregation = gridAggregationMethodEdit.findData(aggregationString);
    if (indexAggregation != -1)
       gridAggregationMethodEdit.setCurrentIndex(indexAggregation);

    layoutAggregation->addWidget(&gridAggregationMethodEdit);
    layoutMain->addLayout(layoutAggregation);


    QLabel*
    algorithmEdit;

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
