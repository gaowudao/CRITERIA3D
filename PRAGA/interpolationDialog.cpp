#include <QtWidgets>

#include "interpolationDialog.h"
#include "meteoGrid.h"


InterpolationDialog::InterpolationDialog(QSettings *settings, Crit3DInterpolationSettings *myInterpolationSetting)
{
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
    topographicDistance = new QCheckBox(tr("use topographic distance"));
    topographicDistance->setChecked(_interpolationSettings->getUseTAD());
    layoutMain->addWidget(topographicDistance);

    // dew point
    useDewPointEdit = new QCheckBox(tr("interpolate relative humdity using dew point"));
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
         chkProxy->setChecked(true);
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

void InterpolationDialog::accept()
{
    if (minRegressionR2Edit.text().isEmpty())
    {
        QMessageBox::information(NULL, "Missing R2", "insert minimum regression R2");
        return;
    }

    Crit3DProxyCombination myCombination;

    for (int i = 0; i < proxy.size(); i++)
    {
        if (proxy[i]->isChecked())
            myCombination.getIndexProxy().push_back(i);
    }

    _interpolationSettings->setSelectedCombination(myCombination);

}
