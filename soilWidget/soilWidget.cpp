#include "commonConstants.h"
#include "soilWidget.h"
#include "soilDbTools.h"
#include "utilities.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>


SoilWidget::SoilWidget()
{
    this->setWindowTitle(QStringLiteral("Soil"));
    this->resize(800, 600);

    // layout
    soilCombo = new QComboBox();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(soilCombo);
    layout->setAlignment(Qt::AlignTop);
    this->setLayout(layout);

    // menu
    QMenuBar* menuBar = new QMenuBar();
    QMenu *fileMenu = new QMenu("File");
    menuBar->addMenu(fileMenu);

    QAction* LoadSoil = new QAction(tr("&Load soil data"), this);
    connect(LoadSoil, &QAction::triggered, this, &SoilWidget::on_actionLoadSoil);
    fileMenu->addAction(LoadSoil);

    this->layout()->setMenuBar(menuBar);
}


void SoilWidget::on_actionLoadSoil()
{
    QString dbName = QFileDialog::getOpenFileName(this, tr("Load soil data"), "", tr("SQLite files (*.db)"));
    if (dbName == "") return;

    soilCombo->clear();

    QString error;
    if (! loadAllSoils(dbName, &soilList, soilClassList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    for (unsigned int i = 0; i < soilList.size(); i++)
    {
        soilCombo->addItem(QString::fromStdString(soilList[i].code));
    }

    if(error != "")
    {
        QMessageBox::information(nullptr, "Warning", error);
    }
}



void SoilWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    //TODO
}

