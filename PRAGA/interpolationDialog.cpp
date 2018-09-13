#include <QtWidgets>

#include "interpolationDialog.h"


InterpolationDialog::InterpolationDialog(QSettings *settings, Crit3DInterpolationSettings *myInterpolationSetting)
{
    _interpolationSettings = myInterpolationSetting;

    setWindowTitle(tr("Interpolation settings"));
    setFixedSize(650,700);
    QVBoxLayout *mainLayout = new QVBoxLayout;

//    QComboBox algorithmEdit;
//    QLineEdit minRegressionR2Edit;
//    QCheckBox lapseRateCodeEdit;
//    QCheckBox thermalInversionEdit;
//    QCheckBox optimalDetrendingEdit;
//    QCheckBox topographicDistance;
//    QCheckBox useDewPointEdit;
//    QComboBox gridAggregationMethodEdit;

    //algorithm
    QLabel *algorithmLabel = new QLabel(tr("algorithm"));
    QHBoxLayout *algorithmLayout = new QHBoxLayout;
    QHBoxLayout *proxyLayout = new QHBoxLayout;

    std::map<std::string, TInterpolationMethod>::const_iterator it;
    for (it = interpolationMethodNames.begin(); it != interpolationMethodNames.end(); ++it)
        algorithmEdit.addItem(QString::fromStdString(it->first), QString::fromStdString(it->first));

    QString algorithmString = QString::fromStdString(getKeyStringInterpolationMethod(_interpolationSettings->getInterpolationMethod()));
    int indexAlgorithm = algorithmEdit.findData(algorithmString);
    if (indexAlgorithm != -1)
       algorithmEdit.setCurrentIndex(indexAlgorithm);

    algorithmLayout->addWidget(algorithmLabel);
    algorithmLayout->addWidget(&algorithmEdit);
    mainLayout->addLayout(algorithmLayout);

    // proxies
    for (int i = 0; i < _interpolationSettings->getProxyNr(); i++)
    {
         Crit3DProxyInterpolation* myProxy = _interpolationSettings->getProxy(i);
         QCheckBox *chkProxy = new QCheckBox(QString::fromStdString(myProxy->getName()));
         proxyLayout->addWidget(chkProxy);
         proxy.append(chkProxy);
    }
    mainLayout->addLayout(proxyLayout);

    //buttons
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

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
