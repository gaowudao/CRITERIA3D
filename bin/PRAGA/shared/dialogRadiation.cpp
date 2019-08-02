#include <QtWidgets>

#include "project.h"
#include "radiationSettings.h"
#include "dialogRadiation.h"


DialogRadiation::DialogRadiation(Project* myProject)
{
    project_ = myProject;

    setWindowTitle(tr("Radiation settings"));
    QHBoxLayout* layoutMain = new QHBoxLayout();
    QVBoxLayout* layoutMainLeft = new QVBoxLayout();
    QVBoxLayout* layoutMainRight = new QVBoxLayout();

    QHBoxLayout* layoutAlgorithm = new QHBoxLayout();
    comboAlgorithm = new QComboBox();
    checkShadowing = new QCheckBox("shadowing");
    checkRealSky = new QCheckBox("real sky");
    comboRealSky = new QComboBox();
    comboLinkeMode = new QComboBox();
    groupLinke = new QGroupBox("Linke turbidity factor");
    editLinke = new QLineEdit(QString::number(double(project_->radSettings.getLinke())));
    buttonLinke = new QPushButton("Load Linke map...");
    editLinkeMap = new QLineEdit();
    comboAlbedoMode = new QComboBox();
    buttonAlbedo = new QPushButton("Load albedo map...");
    editAlbedoMap = new QLineEdit();
    editAlbedo = new QLineEdit(QString::number(double(project_->radSettings.getAlbedo())));
    comboTiltMode = new QComboBox();
    editTilt = new QLineEdit(QString::number(double(project_->radSettings.getTilt())));
    editAspect = new QLineEdit(QString::number(double(project_->radSettings.getAspect())));


    // --------------------------------------------------------
    // algorithm
    QLabel* labelAlgorithm = new QLabel("algorithm");
    layoutAlgorithm->addWidget(labelAlgorithm);

    connect(comboAlgorithm, &QComboBox::currentTextChanged, [=](const QString &newVar){ this->updateAlgorithm(newVar); });
    std::map<std::string, TradiationAlgorithm>::const_iterator itAlgo;
    for (itAlgo = radAlgorithmToString.begin(); itAlgo != radAlgorithmToString.end(); ++itAlgo)
        comboAlgorithm->addItem(QString::fromStdString(itAlgo->first), QString::fromStdString(itAlgo->first));

    QString radString = QString::fromStdString(getKeyStringRadAlgorithm(project_->radSettings.getAlgorithm()));
    int indexCombo = comboAlgorithm->findData(radString);
    if (indexCombo != -1)
       comboAlgorithm->setCurrentIndex(indexCombo);

    layoutAlgorithm->addWidget(comboAlgorithm);

    layoutMainLeft->addLayout(layoutAlgorithm);

    // -------------------------------------------------------
    // shadowing
    checkShadowing->setChecked(project_->radSettings.getShadowing());
    layoutMainLeft->addWidget(checkShadowing);

    // ----------------------------------------------------
    // transmissivity settings
    QGroupBox* groupTransmissivity = new QGroupBox("real sky settings");
    QVBoxLayout* layoutTransmissivity = new QVBoxLayout();
    QVBoxLayout* layoutTransSettings = new QVBoxLayout();

    connect(checkRealSky, &QCheckBox::clicked, [=](){ this->updateRealSky(); });
    checkRealSky->setChecked(project_->radSettings.getRealSky());
    layoutTransSettings->addWidget(checkRealSky);

    QHBoxLayout* layoutTransAlgorithm = new QHBoxLayout();
    QLabel* labelTransAlgorithm = new QLabel("algorithm");
    layoutTransAlgorithm->addWidget(labelTransAlgorithm);

    connect(comboRealSky,  &QComboBox::currentTextChanged, [=](const QString &newVar){ this->updateRealSkyAlgorithm(newVar); });

    std::map<std::string, TradiationRealSkyAlgorithm>::const_iterator itRealSky;
    for (itRealSky = realSkyAlgorithmToString.begin(); itRealSky != realSkyAlgorithmToString.end(); ++itRealSky)
        comboRealSky->addItem(QString::fromStdString(itRealSky->first), QString::fromStdString(itRealSky->first));

    QString realSkyString = QString::fromStdString(getKeyStringRealSky(project_->radSettings.getRealSkyAlgorithm()));
    indexCombo = comboRealSky->findData(realSkyString);
    if (indexCombo != -1)
       comboRealSky->setCurrentIndex(indexCombo);

    layoutTransAlgorithm->addWidget(comboRealSky);
    layoutTransSettings->addLayout(layoutTransAlgorithm);

    QLabel* labelTransClear = new QLabel("clear sky transmissivity");
    editTransClearSky = new QLineEdit(QString::number(double(project_->radSettings.getClearSky())));
    QDoubleValidator* doubleValClearSky = new QDoubleValidator(0.0, 1.0, 2, this);
    editTransClearSky->setValidator(doubleValClearSky);
    layoutTransSettings->addWidget(labelTransClear);
    layoutTransSettings->addWidget(editTransClearSky);
    layoutTransmissivity->addLayout(layoutTransSettings);

    QVBoxLayout* layoutLinke = new QVBoxLayout();

    QHBoxLayout* layoutLinkeMode = new QHBoxLayout();
    QLabel* labelLinkeMode = new QLabel("mode");
    layoutLinkeMode->addWidget(labelLinkeMode);

    connect(comboLinkeMode,  &QComboBox::currentTextChanged, [=](const QString &newVar){ this->updateLinkeMode(newVar); });

    std::map<std::string, TparameterMode>::const_iterator itParam;
    for (itParam = paramModeToString.begin(); itParam != paramModeToString.end(); ++itParam)
        comboLinkeMode->addItem(QString::fromStdString(itParam->first), QString::fromStdString(itParam->first));

    QString linkeString = QString::fromStdString(getKeyStringParamMode(project_->radSettings.getLinkeMode()));
    indexCombo = comboLinkeMode->findData(radString);
    if (indexCombo != -1)
       comboLinkeMode->setCurrentIndex(indexCombo);

    layoutLinkeMode->addWidget(comboLinkeMode);
    layoutLinke->addLayout(layoutLinkeMode);

    QHBoxLayout* layoutLinkeFixed = new QHBoxLayout();
    QLabel* linkeFixed = new QLabel("fixed Linke value");
    layoutLinkeFixed->addWidget(linkeFixed);
    QDoubleValidator* doubleValLinke = new QDoubleValidator(0.0, 10.0, 1, this);
    editLinke->setValidator(doubleValLinke);
    layoutLinkeFixed->addWidget(editLinke);
    layoutLinke->addLayout(layoutLinkeFixed);

    QHBoxLayout* layoutLinkeMap = new QHBoxLayout();
    editLinkeMap->setEnabled(false);
    layoutLinkeMap->addWidget(editLinkeMap);
    layoutLinkeMap->addWidget(buttonLinke);
    layoutLinke->addLayout(layoutLinkeMap);

    connect(buttonLinke, &QPushButton::clicked, [=](){ this->loadLinke(); });

    groupLinke->setLayout(layoutLinke);
    layoutTransmissivity->addWidget(groupLinke);
    groupTransmissivity->setLayout(layoutTransmissivity);
    layoutMainLeft->addWidget(groupTransmissivity);

    // -------------------------------------------------------------
    // albedo
    QGroupBox* groupAlbedo = new QGroupBox("albedo");
    QVBoxLayout* layoutAlbedo = new QVBoxLayout();

    QHBoxLayout* layoutAlbedoMode = new QHBoxLayout();
    QLabel* labelAlbedoMode = new QLabel("mode");
    layoutAlbedoMode->addWidget(labelAlbedoMode);

    connect(comboAlbedoMode,  &QComboBox::currentTextChanged, [=](const QString &newVar){ this->updateAlbedoMode(newVar); });

    for (itParam = paramModeToString.begin(); itParam != paramModeToString.end(); ++itParam)
        comboAlbedoMode->addItem(QString::fromStdString(itParam->first), QString::fromStdString(itParam->first));

    QString albedoString = QString::fromStdString(getKeyStringParamMode(project_->radSettings.getAlbedoMode()));
    indexCombo = comboAlbedoMode->findData(radString);
    if (indexCombo != -1)
       comboAlbedoMode->setCurrentIndex(indexCombo);

    layoutAlbedoMode->addWidget(comboAlbedoMode);
    layoutAlbedo->addLayout(layoutAlbedoMode);

    QHBoxLayout* layoutAlbedoFixed = new QHBoxLayout();
    QLabel* albedoFixed = new QLabel("fixed albedo value");
    layoutAlbedoFixed->addWidget(albedoFixed);
    QDoubleValidator* doubleValAlbedo = new QDoubleValidator(0.0, 1.0, 2, this);
    editAlbedo->setValidator(doubleValAlbedo);
    layoutAlbedoFixed->addWidget(editAlbedo);
    layoutAlbedo->addLayout(layoutAlbedoFixed);

    QHBoxLayout* layoutAlbedoMap = new QHBoxLayout();
    editAlbedoMap->setEnabled(false);
    layoutAlbedoMap->addWidget(editAlbedoMap);
    layoutAlbedoMap->addWidget(buttonAlbedo);
    layoutAlbedo->addLayout(layoutAlbedoMap);

    connect(buttonAlbedo, &QPushButton::clicked, [=](){ this->loadAlbedo(); });

    groupAlbedo->setLayout(layoutAlbedo);
    layoutMainRight->addWidget(groupAlbedo);

    // -------------------------------------------------------------
    // tilt
    QGroupBox* groupTilt = new QGroupBox("tilt");
    QVBoxLayout* layoutTilt = new QVBoxLayout();

    QHBoxLayout* layoutTiltMode = new QHBoxLayout();
    QLabel* labelTiltMode = new QLabel("mode");
    layoutTiltMode->addWidget(labelTiltMode);

    connect(comboTiltMode,  &QComboBox::currentTextChanged, [=](const QString &newVar){ this->updateTiltMode(newVar); });

    std::map<std::string, TtiltMode>::const_iterator itTilt;
    for (itTilt = tiltModeToString.begin(); itTilt != tiltModeToString.end(); ++itTilt)
        comboTiltMode->addItem(QString::fromStdString(itTilt->first), QString::fromStdString(itTilt->first));

    QString tiltString = QString::fromStdString(getKeyStringTiltMode(project_->radSettings.getTiltMode()));
    indexCombo = comboTiltMode->findData(tiltString);
    if (indexCombo != -1)
       comboTiltMode->setCurrentIndex(indexCombo);

    layoutTiltMode->addWidget(comboTiltMode);
    layoutTilt->addLayout(layoutTiltMode);

    QHBoxLayout* layoutTiltFixed = new QHBoxLayout();
    QLabel* tiltFixed = new QLabel("fixed tilt value");
    layoutTiltFixed->addWidget(tiltFixed);
    QDoubleValidator* doubleValTilt = new QDoubleValidator(0.0, 90.0, 1, this);
    editLinke->setValidator(doubleValTilt);
    layoutTiltFixed->addWidget(editTilt);
    layoutTilt->addLayout(layoutTiltFixed);

    QHBoxLayout* layoutAspectFixed = new QHBoxLayout();
    QLabel* aspectFixed = new QLabel("fixed aspect value");
    layoutAspectFixed->addWidget(aspectFixed);
    QDoubleValidator* doubleValAspect = new QDoubleValidator(0.0, 360.0, 1, this);
    editLinke->setValidator(doubleValAspect);
    layoutAspectFixed->addWidget(editAspect);
    layoutTilt->addLayout(layoutAspectFixed);

    groupTilt->setLayout(layoutTilt);

    layoutMainRight->addWidget(groupTilt);

    layoutMain->addLayout(layoutMainLeft);
    layoutMain->addLayout(layoutMainRight);

    // --------------------------------------------------------------
    // buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layoutMain->addWidget(buttonBox);

    setLayout(layoutMain);

    exec();
}

void DialogRadiation::updateAlgorithm(const QString myString)
{
    TradiationAlgorithm myAlgorithm = radAlgorithmToString.at(myString.toStdString());
    if (myAlgorithm == RADIATION_ALGORITHM_BROOKS)
    {
        groupLinke->setEnabled(false);
    }
    else if (myAlgorithm == RADIATION_ALGORITHM_RSUN)
    {
        groupLinke->setEnabled(true);
    }
}

void DialogRadiation::updateRealSkyAlgorithm(const QString myString)
{
    TradiationRealSkyAlgorithm myAlgorithm = realSkyAlgorithmToString.at(myString.toStdString());
    groupLinke->setEnabled(myAlgorithm == RADIATION_REALSKY_LINKE);
}

void DialogRadiation::updateLinkeMode(const QString myString)
{
    TparameterMode myMode = paramModeToString.at(myString.toStdString());
    buttonLinke->setEnabled(myMode == PARAM_MODE_MAP);
    editLinke->setEnabled(myMode == PARAM_MODE_FIXED);
}

void DialogRadiation::updateAlbedoMode(const QString myString)
{
    TparameterMode myMode = paramModeToString.at(myString.toStdString());
    buttonAlbedo->setEnabled(myMode == PARAM_MODE_MAP);
    editAlbedo->setEnabled(myMode == PARAM_MODE_FIXED);
}

void DialogRadiation::updateTiltMode(const QString myString)
{
    TtiltMode myMode = tiltModeToString.at(myString.toStdString());
    editTilt->setEnabled(myMode == TILT_TYPE_FIXED);
    editAspect->setEnabled(myMode == TILT_TYPE_FIXED);
}

void DialogRadiation::updateRealSky()
{
    comboRealSky->setEnabled(checkRealSky->isChecked());
    comboLinkeMode->setEnabled(checkRealSky->isChecked());
    editLinke->setEnabled(checkRealSky->isChecked());
    buttonLinke->setEnabled(checkRealSky->isChecked());
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
