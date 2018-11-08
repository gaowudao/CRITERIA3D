
#include <QtWidgets>

#include "project.h"
#include "utilities.h"
#include "interpolationDialog.h"

InterpolationDialog::InterpolationDialog(Project *myProject)
{
    _interpolationSettings = &(myProject->interpolationSettings);
    _paramSettings = myProject->settings;

    setWindowTitle(tr("Interpolation settings"));
    QVBoxLayout *layoutMain = new QVBoxLayout;
    QVBoxLayout *layoutDetrending = new QVBoxLayout();

    // grid aggregation
    QHBoxLayout *layoutAggregation = new QHBoxLayout;
    QLabel *labelAggregation = new QLabel(tr("aggregation method"));
    layoutAggregation->addWidget(labelAggregation);

    std::map<std::string, gridAggregationMethod>::const_iterator itAggr;
    for (itAggr = gridAggregationMethodNames.begin(); itAggr != gridAggregationMethodNames.end(); ++itAggr)
        gridAggregationMethodEdit.addItem(QString::fromStdString(itAggr->first), QString::fromStdString(itAggr->first));

    QString aggregationString = QString::fromStdString(getKeyStringAggregationMethod(_interpolationSettings->getMeteoGridAggrMethod()));
    int indexAggregation = gridAggregationMethodEdit.findData(aggregationString);
    if (indexAggregation != -1)
       gridAggregationMethodEdit.setCurrentIndex(indexAggregation);

    layoutAggregation->addWidget(&gridAggregationMethodEdit);

    // topographic distances
    topographicDistanceEdit = new QCheckBox(tr("use topographic distance"));
    topographicDistanceEdit->setChecked(_interpolationSettings->getUseTAD());
    layoutMain->addWidget(topographicDistanceEdit);

    // dew point
    useDewPointEdit = new QCheckBox(tr("interpolate relative humidity using dew point"));
    useDewPointEdit->setChecked(_interpolationSettings->getUseDewPoint());
    layoutMain->addWidget(useDewPointEdit);

    // R2
    QLabel *labelMinR2 = new QLabel(tr("minimum regression R2"));
    QDoubleValidator *doubleValR2 = new QDoubleValidator(0.0, 1.0, 2, this);
    doubleValR2->setNotation(QDoubleValidator::StandardNotation);
    minRegressionR2Edit.setFixedWidth(30);
    minRegressionR2Edit.setValidator(doubleValR2);
    minRegressionR2Edit.setText(QString::number(_interpolationSettings->getMinRegressionR2()));
    layoutDetrending->addWidget(labelMinR2);
    layoutDetrending->addWidget(&minRegressionR2Edit);

    // lapse rate code
    lapseRateCodeEdit = new QCheckBox(tr("use lapse rate code"));
    lapseRateCodeEdit->setChecked(_interpolationSettings->getUseLapseRateCode());
    layoutDetrending->addWidget(lapseRateCodeEdit);

    // thermal inversion
    thermalInversionEdit = new QCheckBox(tr("thermal inversion"));
    thermalInversionEdit->setChecked(_interpolationSettings->getUseThermalInversion());
    layoutDetrending->addWidget(thermalInversionEdit);

    // thermal inversion
    optimalDetrendingEdit = new QCheckBox(tr("optimal detrending"));
    optimalDetrendingEdit->setChecked(_interpolationSettings->getUseBestDetrending());
    layoutDetrending->addWidget(optimalDetrendingEdit);

    //algorithm
    QHBoxLayout *layoutAlgorithm = new QHBoxLayout;
    QLabel *labelAlgorithm = new QLabel(tr("algorithm"));
    layoutAlgorithm->addWidget(labelAlgorithm);

    std::map<std::string, TInterpolationMethod>::const_iterator itAlg;
    for (itAlg = interpolationMethodNames.begin(); itAlg != interpolationMethodNames.end(); ++itAlg)
        algorithmEdit.addItem(QString::fromStdString(itAlg->first), QString::fromStdString(itAlg->first));

    QString algorithmString = QString::fromStdString(getKeyStringInterpolationMethod(_interpolationSettings->getInterpolationMethod()));
    int indexAlgorithm = algorithmEdit.findData(algorithmString);
    if (indexAlgorithm != -1)
       algorithmEdit.setCurrentIndex(indexAlgorithm);

    layoutAlgorithm->addWidget(&algorithmEdit);
    layoutMain->addLayout(layoutAlgorithm);

    // proxies
    QHBoxLayout *layoutProxy = new QHBoxLayout;
    QLabel *labelProxy = new QLabel(tr("temperature detrending proxies"));
    layoutProxy->addWidget(labelProxy);

    for (int i = 0; i < _interpolationSettings->getProxyNr(); i++)
    {
         Crit3DProxy* myProxy = _interpolationSettings->getProxy(i);
         QCheckBox *chkProxy = new QCheckBox(QString::fromStdString(myProxy->getName()));
         chkProxy->setChecked(_interpolationSettings->getSelectedCombination().getValue(i));
         layoutProxy->addWidget(chkProxy);
         proxy.append(chkProxy);
    }
    layoutDetrending->addLayout(layoutProxy);

    layoutMain->addLayout(layoutDetrending);

    //buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layoutMain->addWidget(buttonBox);

    layoutMain->addStretch(1);
    setLayout(layoutMain);

    exec();
}

void InterpolationDialog::writeInterpolationSettings()
{
    _paramSettings->beginGroup("interpolation");
    _paramSettings->setValue("gridAggregationMethod", gridAggregationMethodEdit.itemData(gridAggregationMethodEdit.currentIndex()).toString());
    _paramSettings->setValue("algorithm", algorithmEdit.itemData(algorithmEdit.currentIndex()).toString());
    _paramSettings->setValue("lapseRateCode", lapseRateCodeEdit->isChecked());
    _paramSettings->setValue("thermalInversion", thermalInversionEdit->isChecked());
    _paramSettings->setValue("topographicDistance", topographicDistanceEdit->isChecked());
    _paramSettings->setValue("optimalDetrending", optimalDetrendingEdit->isChecked());
    _paramSettings->setValue("useDewPoint", useDewPointEdit->isChecked());
    _paramSettings->setValue("thermalInversion", thermalInversionEdit->isChecked());
    _paramSettings->setValue("minRegressionR2", minRegressionR2Edit.text());
    _paramSettings->endGroup();

    Crit3DProxy* myProxy;
    for (int i=0; i < _interpolationSettings->getProxyNr(); i++)
    {
        myProxy = _interpolationSettings->getProxy(i);
        _paramSettings->beginGroup("proxy_" + QString::fromStdString(myProxy->getName()));
        _paramSettings->setValue("active", _interpolationSettings->getSelectedCombination().getValue(i));
        _paramSettings->endGroup();
    }
}

void InterpolationDialog::accept()
{
    if (minRegressionR2Edit.text().isEmpty())
    {
        QMessageBox::information(NULL, "Missing R2", "insert minimum regression R2");
        return;
    }

    if (algorithmEdit.currentIndex() == -1)
    {
        QMessageBox::information(NULL, "No algorithm selected", "Choose algorithm");
        return;
    }

    for (int i = 0; i < proxy.size(); i++)
        _interpolationSettings->setValueSelectedCombination(i, proxy[i]->isChecked());


    QString algorithmString = algorithmEdit.itemData(algorithmEdit.currentIndex()).toString();
    _interpolationSettings->setInterpolationMethod(interpolationMethodNames.at(algorithmString.toStdString()));

    QString aggrString = gridAggregationMethodEdit.itemData(gridAggregationMethodEdit.currentIndex()).toString();
    _interpolationSettings->setMeteoGridAggrMethod(gridAggregationMethodNames.at(aggrString.toStdString()));

    _interpolationSettings->setUseTAD(topographicDistanceEdit->isChecked());
    _interpolationSettings->setUseLapseRateCode(lapseRateCodeEdit->isChecked());
    _interpolationSettings->setUseBestDetrending(optimalDetrendingEdit->isChecked());
    _interpolationSettings->setUseThermalInversion(thermalInversionEdit->isChecked());
    _interpolationSettings->setUseDewPoint(useDewPointEdit->isChecked());
    _interpolationSettings->setMinRegressionR2(minRegressionR2Edit.text().toFloat());

    writeInterpolationSettings();

    QDialog::done(QDialog::Accepted);
    return;

}

void ProxyDialog::changedTable()
{
    QSqlDatabase db = _project->meteoPointsDbHandler->getDb();
    _field.addItems(getFields(&db, _table.currentText()));
}

void ProxyDialog::changedProxy()
{
    if (_proxyCombo.count() == 0) return;
    Crit3DProxy *myProxy = &(_proxy.at(_proxyCombo.currentIndex()));
    int index = _table.findText(QString::fromStdString(myProxy->getProxyTable()));
    _table.setCurrentIndex(index);
    index = _field.findText(QString::fromStdString(myProxy->getProxyField()));
    _field.setCurrentIndex(index);
    _proxyGridName.setText(QString::fromStdString(myProxy->getGridName()));
    _forQuality.setChecked(myProxy->getForQualityControl());
}

void ProxyDialog::getGridFile()
{
    QString qFileName = QFileDialog::getOpenFileName();
    if (qFileName == "") return;
    qFileName = qFileName.left(qFileName.length()-4);

    std::string fileName = qFileName.toStdString();
    std::string error_;
    gis::Crit3DRasterGrid* grid_ = new gis::Crit3DRasterGrid();
    if (gis::readEsriGrid(fileName, grid_, &error_))
        _proxyGridName.setText(qFileName);
    else
        QMessageBox::information(NULL, "Error", "Error opening " + qFileName);

    grid_ = NULL;
}

void ProxyDialog::redrawProxies()
{
    _proxyCombo.blockSignals(true);
    _proxyCombo.clear();
    for (int i = 0; i < _proxy.size(); i++)
         _proxyCombo.addItem(QString::fromStdString(_proxy.at(i).getName()));
    _proxyCombo.blockSignals(false);
}

void ProxyDialog::addProxy()
{
    bool isValid;
    QString proxyName = QInputDialog::getText(this, tr("New proxy"),
                                         tr("Insert proxy name:"), QLineEdit::Normal,
                                         "", &isValid);
    if (isValid && !proxyName.isEmpty())
    {
        Crit3DProxy myProxy;
        myProxy.setName(proxyName.toStdString());
        _proxy.push_back(myProxy);
        redrawProxies();
        _proxyCombo.setCurrentIndex(_proxyCombo.count()-1);
    }

    return;
}

void ProxyDialog::deleteProxy()
{
    _proxy.erase(_proxy.begin() + _proxyCombo.currentIndex());
    redrawProxies();
}

void ProxyDialog::saveProxy()
{
    if (_proxyCombo.currentIndex() == -1)
        return;

    Crit3DProxy* myProxy;
    myProxy = &_proxy.at(_proxyCombo.currentIndex());

    if (_table.currentIndex() != -1)
        myProxy->setProxyTable(_table.currentText().toStdString());

    if (_field.currentIndex() != -1)
        myProxy->setProxyField(_field.currentText().toStdString());

    if (_proxyGridName.text() != "")
        myProxy->setGridName(_proxyGridName.text().toStdString());

    myProxy->setForQualityControl(_forQuality.isChecked());
}

ProxyDialog::ProxyDialog(Project *myProject)
{
    QVBoxLayout *layoutMain = new QVBoxLayout();
    QHBoxLayout *layoutProxyCombo = new QHBoxLayout();
    QVBoxLayout *layoutProxy = new QVBoxLayout();
    QVBoxLayout *layoutPointValues = new QVBoxLayout();
    QVBoxLayout *layoutGrid = new QVBoxLayout();

    this->resize(300, 100);

    _project = myProject;
    _proxy = _project->interpolationSettings.getCurrentProxy();

    setWindowTitle(tr("Interpolation proxy"));

    // proxy list
    QLabel *labelProxyList = new QLabel(tr("proxy list"));
    layoutProxy->addWidget(labelProxyList);
    _proxyCombo.clear();
    redrawProxies();

    connect(&_proxyCombo, &QComboBox::currentTextChanged, [=](){ this->changedProxy(); });
    layoutProxyCombo->addWidget(&_proxyCombo);

    QPushButton *_add = new QPushButton("add");
    layoutProxyCombo->addWidget(_add);
    connect(_add, &QPushButton::clicked, [=](){ this->addProxy(); });

    QPushButton *_delete = new QPushButton("delete");
    layoutProxyCombo->addWidget(_delete);
    connect(_delete, &QPushButton::clicked, [=](){ this->deleteProxy(); });

    QPushButton *_saveProxy = new QPushButton("save");
    layoutProxyCombo->addWidget(_saveProxy);
    connect(_saveProxy, &QPushButton::clicked, [=](){ this->saveProxy(); });

    layoutProxy->addLayout(layoutProxyCombo);
    layoutMain->addLayout(layoutProxy);

    QLabel *labelTableList = new QLabel(tr("table for point values"));
    layoutPointValues->addWidget(labelTableList);
    QStringList tables_ = _project->meteoPointsDbHandler->getDb().tables();
    for (int i=0; i < tables_.size(); i++)
        _table.addItem(tables_.at(i));

    layoutPointValues->addWidget(&_table);
    connect(&_table, &QComboBox::currentTextChanged, [=](){ this->changedTable(); });

    QLabel *labelFieldList = new QLabel(tr("field for point values"));
    layoutPointValues->addWidget(labelFieldList);
    layoutPointValues->addWidget(&_field);
    layoutMain->addLayout(layoutPointValues);

    // grid
    QLabel *labelGrid = new QLabel(tr("proxy grid"));
    layoutGrid->addWidget(labelGrid);
    QPushButton *_selectGrid = new QPushButton("Select file");
    layoutGrid->addWidget(_selectGrid);
    _proxyGridName.setEnabled(false);
    layoutGrid->addWidget(&_proxyGridName);
    connect(_selectGrid, &QPushButton::clicked, [=](){ this->getGridFile(); });
    layoutMain->addLayout(layoutGrid);

    // quality control
    _forQuality.setText("use for quality control");
    layoutMain->addWidget(&_forQuality);

    // buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layoutMain->addWidget(buttonBox);

    QPushButton *_save = new QPushButton("Save proxies to settings");
    connect(_save, &QPushButton::clicked, [=](){ this->writeProxies(); });
    layoutMain->addWidget(_save);

    layoutMain->addStretch(1);
    setLayout(layoutMain);

    if (_proxyCombo.count() > 0)
    {
        _proxyCombo.setCurrentIndex(0);
        changedProxy();
    }

    exec();
}

bool ProxyDialog::checkProxies(std::string *error)
{
    for (int i=0; i < _proxy.size(); i++)
    {
        if (!_project->checkProxy(_proxy.at(i).getName(), _proxy.at(i).getGridName(), _proxy.at(i).getProxyTable(), _proxy.at(i).getProxyField(), error))
            return false;
    }

    return true;
}

void ProxyDialog::saveProxies()
{
    for (int i=0; i < _proxy.size(); i++)
    {
        _project->addProxy(_proxy.at(i).getName(), _proxy.at(i).getGridName(),
                           _proxy.at(i).getProxyTable(), _proxy.at(i).getProxyField(),
                            _proxy.at(i).getForQualityControl(), true);
    }
}

void ProxyDialog::writeProxies()
{
    for (int i=0; i < _proxy.size(); i++)
    {
        _project->settings->beginGroup("proxy_" + QString::fromStdString(_proxy.at(i).getName()));
        _project->settings->setValue("table", QString::fromStdString(_proxy.at(i).getProxyTable()));
        _project->settings->setValue("field", QString::fromStdString(_proxy.at(i).getProxyField()));
        _project->settings->setValue("raster", QString::fromStdString(_proxy.at(i).getGridName()));
        _project->settings->endGroup();
    }
}

void ProxyDialog::accept()
{
    // check proxies
    std::string error;
    if (checkProxies(&error))
    {
        saveProxies();
        QDialog::done(QDialog::Accepted);
    }
    else
        QMessageBox::information(NULL, "Proxy error", QString::fromStdString(error));

    return;
}
