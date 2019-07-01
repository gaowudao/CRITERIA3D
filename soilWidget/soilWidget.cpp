#include "commonConstants.h"
#include "soilWidget.h"
#include "soilDbTools.h"
#include "utilities.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
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
    this->resize(600, 600);

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
    if (! loadSoilData(dbName, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
    }
    else if(error != "")
    {
        QMessageBox::information(nullptr, "Warning", error);
    }
    else
    {
        for (unsigned int i = 0; i < soilList.size(); i++)
        {
            soilCombo->addItem(QString::fromStdString(soilList[i].code));
        }
    }
}


bool SoilWidget::loadSoilData(QString dbName, QString* error)
{
    soilList.clear();

    QSqlDatabase dbSoil = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    dbSoil.setDatabaseName(dbName);

    if (!dbSoil.open())
    {
       *error = "Connection with database fail";
       return false;
    }

    if (! loadVanGenuchtenParameters(soilClassList, &dbSoil, error))
    {
        return false;
    }

    // query soil list
    QString queryString = "SELECT id_soil, soil_code FROM soils";
    QSqlQuery query = dbSoil.exec(queryString);

    query.first();
    if (! query.isValid())
    {
        if (query.lastError().number() > 0)
        {
            *error = query.lastError().text();
        }
        else
        {
            *error = "Error in reading table soils";
        }
        return false;
    }

    // load soil properties
    QString soilCode;
    int idSoil;
    QString wrongSoils = "";
    do
    {
        getValue(query.value("id_soil"), &idSoil);
        getValue(query.value("soil_code"), &soilCode);
        if (idSoil != NODATA && soilCode != "")
        {
            soil::Crit3DSoil *mySoil = new soil::Crit3DSoil;
            if (loadSoil(&dbSoil, soilCode, mySoil, soilClassList, error))
            {
                mySoil->id = idSoil;
                mySoil->code = soilCode.toStdString();
                soilList.push_back(*mySoil);
            }
            else
            {
                wrongSoils += *error + "\n";
            }
        }
    } while(query.next());

    if (soilList.size() == 0)
    {
       *error = "Missing soil properties";
       return false;
    }
    else if (wrongSoils != "")
    {
        *error = wrongSoils;
    }

    return true;
}


void SoilWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    //TODO
}

