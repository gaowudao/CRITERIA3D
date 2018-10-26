
#include <QtWidgets>

#include "interpolationDialog.h"
#include "meteoGrid.h"
#include "dbMeteoPoints.h"

InterpolationDialog::InterpolationDialog(QSettings *settings, Crit3DInterpolationSettings *myInterpolationSetting)
{
    _paramSettings = settings;
    _interpolationSettings = myInterpolationSetting;

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
         Crit3DProxyInterpolation* myProxy = _interpolationSettings->getProxy(i);
         QCheckBox *chkProxy = new QCheckBox(QString::fromStdString(myProxy->getName()));
         chkProxy->setChecked(_interpolationSettings->getSelectedCombination().getValue(i));
         layoutProxy->addWidget(chkProxy);
         proxy.append(chkProxy);
    }
    layoutDetrending->addLayout(layoutProxy);

    layoutMain->addLayout(layoutDetrending);

    //buttons
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
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

    Crit3DProxyInterpolation* myProxy;
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

void ProxyDialog::showProxyProperties()
{
    Crit3DProxy myProxy = _meteoPointsHandler->getProxyMeteoPoint().at(_proxy.currentIndex());
    _proxyGridName.setText(QString::fromStdString(myProxy.getGridName()));

}

ProxyDialog::ProxyDialog(QSettings *settings, Crit3DMeteoPointsDbHandler *myMeteoPointsHandler)
{
    _paramSettings = settings;
    _meteoPointsHandler = myMeteoPointsHandler;

    setWindowTitle(tr("Interpolation proxy"));
    QVBoxLayout *layoutMain = new QVBoxLayout;

    QLabel *labelProxyList = new QLabel(tr("proxy list"));
    layoutMain->addWidget(labelProxyList);

    std::vector <Crit3DProxyMeteoPoint> myProxies = _meteoPointsHandler->getProxyMeteoPoint();
    for (int i = 0; i < myProxies.size(); i++)
    {
         _proxy.addItem(QString::fromStdString(myProxies.at(i).getName()));
    }
    if (myProxies.size() > 0)
        _proxy.setCurrentIndex(0);
    layoutMain->addWidget(&_proxy);
    connect(&_proxy, &QComboBox::currentTextChanged, [=](){ this->showProxyProperties(); });

    QLabel *labelTableList = new QLabel(tr("table for point values"));
    layoutMain->addWidget(labelTableList);

    layoutMain->addWidget(&_table);
    layoutMain->addWidget(&_field);

    _proxyGridName.setEnabled(false);
    layoutMain->addWidget(&_proxyGridName);

    //buttons
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layoutMain->addWidget(buttonBox);

    layoutMain->addStretch(1);
    setLayout(layoutMain);

    exec();
}


void ProxyDialog::accept()
{
    QDialog::done(QDialog::Accepted);
    return;
}
