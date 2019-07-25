/*!
    CRITERIA3D

    \copyright 2016 Fausto Tomei, Gabriele Antolini, Laura Costantini
    Alberto Pistocchi, Marco Bittelli, Antonio Volta

    You should have received a copy of the GNU General Public License
    along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.

    This file is part of CRITERIA3D.
    CRITERIA3D has been developed under contract issued by A.R.P.A. Emilia-Romagna

    CRITERIA3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CRITERIA3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with CRITERIA3D.  If not, see <http://www.gnu.org/licenses/>.

    contacts:
    fausto.tomei@gmail.com
    ftomei@arpae.it
*/

#include "soilWidget.h"
#include "soilDbTools.h"
#include "commonConstants.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QFileDialog>
#include <QLayout>
#include <QMenu>
#include <QLabel>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QDebug> //debug


Crit3DSoilWidget::Crit3DSoilWidget()
{
    dbSoilType = DB_SQLITE;

    this->setWindowTitle(QStringLiteral("Soil"));
    this->resize(1400, 800);

    // layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *soilLayout = new QHBoxLayout();
    QVBoxLayout *texturalLayout = new QVBoxLayout();

    QPixmap pic("../../DOC/img/textural_soil.png");
    QLabel *label = new QLabel();
    label->setPixmap (pic);

    soilListComboBox.setFixedWidth(pic.size().width());

    mainLayout->addWidget(&soilListComboBox);
    mainLayout->addLayout(soilLayout);
    mainLayout->setAlignment(Qt::AlignTop);

    texturalLayout->addWidget(label);
    texturalLayout->setAlignment(Qt::AlignTop);


    soilLayout->addLayout(texturalLayout);
    tabWidget = new QTabWidget;
    horizonsTab = new TabHorizons();
    wrDataTab = new TabWaterRetentionData();
    wrCurveTab = new TabWaterRetentionCurve();
    hydraConducCurveTab = new TabHydraulicConductivityCurve();
    tabWidget->addTab(horizonsTab, tr("Horizons"));
    tabWidget->addTab(wrDataTab, tr("Water Retention Data"));
    tabWidget->addTab(wrCurveTab, tr("Water Retention Curve"));
    tabWidget->addTab(hydraConducCurveTab, tr("Hydraulic Conductivity Curve"));

    soilLayout->addWidget(tabWidget);
    this->setLayout(mainLayout);

    // menu
    QMenuBar* menuBar = new QMenuBar();
    QMenu *fileMenu = new QMenu("File");
    QMenu *editMenu = new QMenu("Edit");
    QMenu *optionsMenu = new QMenu("Options");
//    QString  menuStyle(
//               "QMenu::item:checked{"
//               "background-image: url(:/images/checked_hover.png);"
//               "}"
//               "QMenu::item:unchecked {"
//               "background-image: url(:/images/unchecked_hover.png);"
//               "}"
//            );
//    optionsMenu->setStyleSheet(menuStyle);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(optionsMenu);
    this->layout()->setMenuBar(menuBar);

    // actions
    QAction* openSoilDB = new QAction(tr("&Open dbSoil"), this);
    QAction* saveChanges = new QAction(tr("&Save Changes"), this);
    QAction* newSoil = new QAction(tr("&New Soil"), this);
    QAction* deleteSoil = new QAction(tr("&Delete Soil"), this);
    restoreData = new QAction(tr("&Restore Data"), this);
    addHorizon = new QAction(tr("&Add Horizon"), this);
    deleteHorizon = new QAction(tr("&Delete Horizon"), this);
    addHorizon->setEnabled(false);
    deleteHorizon->setEnabled(false);
    restoreData->setEnabled(false);
    useData = new QAction(tr("&Use Water Retention Data"), this);
    airEntry = new QAction(tr("&Air Entry fixed"), this);
    useData->setCheckable(true);
    airEntry->setCheckable(true);
    airEntry->setEnabled(false);

    connect(openSoilDB, &QAction::triggered, this, &Crit3DSoilWidget::on_actionOpenSoilDB);
    connect(saveChanges, &QAction::triggered, this, &Crit3DSoilWidget::on_actionSave);
    connect(newSoil, &QAction::triggered, this, &Crit3DSoilWidget::on_actionNewSoil);
    connect(deleteSoil, &QAction::triggered, this, &Crit3DSoilWidget::on_actionDeleteSoil);
    connect(restoreData, &QAction::triggered, this, &Crit3DSoilWidget::on_actionRestoreData);
    connect(addHorizon, &QAction::triggered, this, &Crit3DSoilWidget::on_actionAddHorizon);
    connect(deleteHorizon, &QAction::triggered, this, &Crit3DSoilWidget::on_actionDeleteHorizon);

    connect(useData, &QAction::triggered, this, &Crit3DSoilWidget::on_actionUseData);
    connect(airEntry, &QAction::triggered, this, &Crit3DSoilWidget::on_actionAirEntry);

    connect(&soilListComboBox, &QComboBox::currentTextChanged, this, &Crit3DSoilWidget::on_actionChooseSoil);

    fileMenu->addAction(openSoilDB);
    fileMenu->addAction(saveChanges);
    editMenu->addAction(newSoil);
    editMenu->addAction(deleteSoil);
    editMenu->addAction(restoreData);
    editMenu->addAction(addHorizon);
    editMenu->addAction(deleteHorizon);
    optionsMenu->addAction(useData);
    optionsMenu->addAction(airEntry);


}


void Crit3DSoilWidget::on_actionOpenSoilDB()
{
    QString dbSoilName = QFileDialog::getOpenFileName(this, tr("Open soil database"), "", tr("SQLite files (*.db)"));
    if (dbSoilName == "") return;

    // open soil db
    QString error;
    if (! openDbSoil(dbSoilName, &dbSoil, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    // load default VG parameters
    if (! loadVanGenuchtenParameters(&dbSoil, textureClassList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    // read soil list
    QStringList soilStringList;
    if (! getSoilList(&dbSoil, &soilStringList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    // show soil list
    this->soilListComboBox.clear();
    for (int i = 0; i < soilStringList.size(); i++)
    {
        this->soilListComboBox.addItem(soilStringList[i]);
    }

}


void Crit3DSoilWidget::on_actionChooseSoil(QString soilCode)
{
    // soilListComboBox has been cleared
    if (soilCode.isEmpty())
    {
        return;
    }
    QString error;
    if (! loadSoil(&dbSoil, soilCode, &mySoil, textureClassList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }
    horizonsTab->clearSelections();
    horizonsTab->insertSoilHorizons(&mySoil, textureClassList);
    addHorizon->setEnabled(true);
    deleteHorizon->setEnabled(true);
    restoreData->setEnabled(true);
}


void Crit3DSoilWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    //TO DO
}

void Crit3DSoilWidget::on_actionNewSoil()
{
    //TO DO
}


void Crit3DSoilWidget::on_actionDeleteSoil()
{
    QString msg;
    if (soilListComboBox.currentText().isEmpty())
    {
        msg = "Select the soil to be deleted";
        QMessageBox::information(nullptr, "Warning", msg);
    }
    else
    {
        QMessageBox::StandardButton confirm;
        msg = "Are you sure you want to delete "+soilListComboBox.currentText()+" ?";
        confirm = QMessageBox::question(nullptr, "Warning", msg, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

        if (confirm == QMessageBox::Yes)
        {
            qDebug() << "Yes was clicked";
            //TO DO
        }
        else
        {
            return;
        }
    }
}

void Crit3DSoilWidget::on_actionUseData()
{
    if (useData->isChecked())
    {
        airEntry->setEnabled(true);
        // TO DO
    }
    else
    {
        airEntry->setEnabled(false);
        // TO DO
    }
}

void Crit3DSoilWidget::on_actionAirEntry()
{
    // TO DO
}

void Crit3DSoilWidget::on_actionSave()
{
    qDebug() << "save changes";
    // TO DO
}

void Crit3DSoilWidget::on_actionAddHorizon()
{

}

void Crit3DSoilWidget::on_actionRestoreData()
{
    horizonsTab->addRowClicked();
}

void Crit3DSoilWidget::on_actionDeleteHorizon()
{
    horizonsTab->removeRowClicked();
}
