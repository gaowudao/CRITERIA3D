#include "settingsDialog.h"

SettingsDialog::SettingsDialog()
{

    setWindowTitle(tr("Parameters"));
    tabWidget = new QTabWidget;
    //tabWidget->addTab(new GeolTab(fileInfo), tr("GEO"));
    //tabWidget->addTab(new QualityTab(fileInfo), tr("QUALITY"));
    //tabWidget->addTab(new ElaborationTab(fileInfo), tr("ELABORATION"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

}
