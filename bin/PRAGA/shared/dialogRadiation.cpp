#include <QtWidgets>

#include "project.h"
#include "radiationSettings.h"
#include "dialogRadiation.h"

DialogRadiation::DialogRadiation(Project* myProject)
{
    project_ = myProject;

    setWindowTitle(tr("Radiation settings"));
    QVBoxLayout* layoutMain = new QVBoxLayout();

    // algorithm
    QHBoxLayout* layoutAlgorithm = new QHBoxLayout();

    QLabel* labelAlgorithm = new QLabel("algorithm");
    layoutAlgorithm->addWidget(labelAlgorithm);

    comboAlgorithm = new QComboBox();
    std::map<std::string, TradiationAlgorithm>::const_iterator itAlgo;
    for (itAlgo = radAlgorithmToString.begin(); itAlgo != radAlgorithmToString.end(); ++itAlgo)
        comboAlgorithm->addItem(QString::fromStdString(itAlgo->first), QString::fromStdString(itAlgo->first));

    QString radString = QString::fromStdString(getKeyStringRadAlgorithm(project_->radSettings.getAlgorithm()));
    int indexCombo = comboAlgorithm->findData(radString);
    if (indexCombo != -1)
       comboAlgorithm->setCurrentIndex(indexCombo);

    layoutAlgorithm->addWidget(comboAlgorithm);

    layoutMain->addLayout(layoutAlgorithm);

    // transmissivity settings
    QGroupBox* groupTransmissivity = new QGroupBox("real sky settings");
    QVBoxLayout* layoutTransmissivity = new QVBoxLayout();
    QVBoxLayout* layoutTransSettings = new QVBoxLayout();

    checkRealSky = new QCheckBox("real sky");
    checkRealSky->setChecked(project_->radSettings.getComputeRealData());
    layoutTransSettings->addWidget(checkRealSky);

    checkUseTotalTransmiss = new QCheckBox("use total transmissivity");
    checkUseTotalTransmiss->setChecked(project_->radSettings.getTransmissivityUseTotal());
    layoutTransSettings->addWidget(checkUseTotalTransmiss);

    QLabel* labelTransClear = new QLabel("clear sky transmissivity");
    editTransClearSky = new QLineEdit(QString::number(double(project_->radSettings.getClearSky())));
    QDoubleValidator *doubleVal = new QDoubleValidator(0.0, 1.0, 2, this);
    editTransClearSky->setValidator(doubleVal);
    layoutTransSettings->addWidget(labelTransClear);
    layoutTransSettings->addWidget(editTransClearSky);
    layoutTransmissivity->addLayout(layoutTransSettings);

    QGroupBox* groupLinke = new QGroupBox("Linke turbidity factor");
    QVBoxLayout* layoutLinke = new QVBoxLayout();

    comboLinkeMode = new QComboBox();
    std::map<std::string, TparameterMode>::const_iterator itParam;
    for (itParam = paramModeToString.begin(); itParam != paramModeToString.end(); ++itParam)
        comboLinkeMode->addItem(QString::fromStdString(itParam->first), QString::fromStdString(itParam->first));

    QString linkeString = QString::fromStdString(getKeyStringParamMode(project_->radSettings.getLinkeMode()));
    indexCombo = comboAlgorithm->findData(radString);
    if (indexCombo != -1)
       comboAlgorithm->setCurrentIndex(indexCombo);

    layoutLinke->addWidget(comboLinkeMode);

    QLabel* linkeFixed = new QLabel("fixed Linke value");
    layoutLinke->addWidget(linkeFixed);
    editLinke = new QLineEdit(QString::number(double(project_->radSettings.getLinke())));
    layoutLinke->addWidget(editLinke);

    QPushButton *buttonLinke = new QPushButton("Load Linke map...");
    layoutLinke->addWidget(buttonLinke);
    connect(buttonLinke, &QPushButton::clicked, [=](){ this->loadLinke(); });

    groupLinke->setLayout(layoutLinke);

    layoutTransmissivity->addWidget(groupLinke);

    groupTransmissivity->setLayout(layoutTransmissivity);

    layoutMain->addWidget(groupTransmissivity);


/*

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

    //buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layoutMain->addWidget(buttonBox);

    layoutMain->addStretch(1);
    setLayout(layoutMain);

    exec();
}

void DialogRadiation::loadLinke()
{
    QString qFileName = QFileDialog::getOpenFileName();
    if (qFileName == "") return;
    qFileName = qFileName.left(qFileName.length()-4);

    std::string fileName = qFileName.toStdString();
    std::string error_;
    gis::Crit3DRasterGrid* grid_ = new gis::Crit3DRasterGrid();
    if (gis::readEsriGrid(fileName, grid_, &error_))
        editLinkeMap->setText(qFileName);
    else
        QMessageBox::information(nullptr, "Error", "Error opening " + qFileName);

    grid_ = nullptr;
}

void DialogRadiation::loadAlbedo()
{
    QString qFileName = QFileDialog::getOpenFileName();
    if (qFileName == "") return;
    qFileName = qFileName.left(qFileName.length()-4);

    std::string fileName = qFileName.toStdString();
    std::string error_;
    gis::Crit3DRasterGrid* grid_ = new gis::Crit3DRasterGrid();
    if (gis::readEsriGrid(fileName, grid_, &error_))
        editAlbedoMap->setText(qFileName);
    else
        QMessageBox::information(nullptr, "Error", "Error opening " + qFileName);

    grid_ = nullptr;
}


void DialogRadiation::accept()
{

}
