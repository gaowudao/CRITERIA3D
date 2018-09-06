#include <QtWidgets>
#include "settingsDialog.h"


GeoTab::GeoTab()
{

}

QualityTab::QualityTab()
{

}

ElaborationTab::ElaborationTab()
{

}

SettingsDialog::SettingsDialog()
{

    setWindowTitle(tr("Parameters"));
    geoTab = new GeoTab();
    qualityTab = new QualityTab();
    elabTab = new ElaborationTab();

    tabWidget = new QTabWidget;
    tabWidget->addTab(geoTab, tr("GEO"));
    tabWidget->addTab(qualityTab, tr("QUALITY"));
    tabWidget->addTab(elabTab, tr("ELABORATION"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

}
