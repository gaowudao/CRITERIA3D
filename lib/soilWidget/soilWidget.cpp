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

#include <math.h>
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
    this->resize(1240, 700);

    // layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *soilLayout = new QHBoxLayout();
    QVBoxLayout *texturalLayout = new QVBoxLayout();
    QGridLayout *infoLayout = new QGridLayout();

    picPath = "../../DOC/img/textural_soil.png";
    pic.load(picPath);
    labelPic = new QLabel();
    labelPic->setPixmap(pic);

    infoGroup = new QGroupBox(tr(""));
    infoGroup->setMaximumWidth(pic.width());
    infoGroup->hide();

    QLabel *soilCodeLabel = new QLabel(tr("Soil code: "));
    soilCodeValue = new QLineEdit();
    soilCodeValue->setReadOnly(true);

    QLabel *satLabel = new QLabel(tr("SAT [m3 m-3]"));
    satValue = new QLineEdit();

    satValue->setReadOnly(true);

    QLabel *fcLabel = new QLabel(tr("FC   [m3 m-3]"));
    fcValue = new QLineEdit();
    fcValue->setReadOnly(true);

    QLabel *wpLabel = new QLabel(tr("WP  [m3 m-3]"));
    wpValue = new QLineEdit();
    wpValue->setReadOnly(true);

    QLabel *awLabel = new QLabel(tr("AW  [m3 m-3]"));
    awValue = new QLineEdit();
    awValue->setReadOnly(true);

    QLabel *potFCLabel = new QLabel(tr("PotFC [kPa]"));
    potFCValue = new QLineEdit();
    potFCValue->setReadOnly(true);

    QLabel *satLegendLabel = new QLabel(tr("SAT = Water content at saturation"));
    QLabel *fcLegendLabel = new QLabel(tr("FC = Water content at Field Capacity"));
    QLabel *wpLegendLabel = new QLabel(tr("WP = Water content at Wilting Point"));
    QLabel *awLegendLabel = new QLabel(tr("AW = Available Water"));
    QLabel *potFCLegendLabel = new QLabel(tr("PotFC = Water Potential at Field Capacity"));

    infoGroup->setTitle(soilName);
    infoLayout->addWidget(soilCodeLabel, 0 , 0);
    infoLayout->addWidget(soilCodeValue, 0 , 1);
    infoLayout->addWidget(satLabel, 1 , 0);
    infoLayout->addWidget(satValue, 1 , 1);
    infoLayout->addWidget(fcLabel, 2 , 0);
    infoLayout->addWidget(fcValue, 2 , 1);
    infoLayout->addWidget(wpLabel, 3 , 0);
    infoLayout->addWidget(wpValue, 3 , 1);
    infoLayout->addWidget(awLabel, 4 , 0);
    infoLayout->addWidget(awValue, 4 , 1);
    infoLayout->addWidget(potFCLabel, 5 , 0);
    infoLayout->addWidget(potFCValue, 5 , 1);

    infoLayout->addWidget(satLegendLabel, 6 , 0);
    infoLayout->addWidget(fcLegendLabel, 7 , 0);
    infoLayout->addWidget(wpLegendLabel, 8 , 0);
    infoLayout->addWidget(awLegendLabel, 9 , 0);
    infoLayout->addWidget(potFCLegendLabel, 10 , 0);
    infoGroup->setLayout(infoLayout);

    soilListComboBox.setFixedWidth(pic.size().width());

    mainLayout->addWidget(&soilListComboBox);
    mainLayout->addLayout(soilLayout);
    mainLayout->setAlignment(Qt::AlignTop);

    texturalLayout->addWidget(labelPic);
    texturalLayout->setAlignment(Qt::AlignTop);
    texturalLayout->addWidget(infoGroup);

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
    QMenu *fittingMenu = new QMenu("Fitting");

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(fittingMenu);
    this->layout()->setMenuBar(menuBar);

    // actions
    QAction* openSoilDB = new QAction(tr("&Open dbSoil"), this);
    saveChanges = new QAction(tr("&Save Changes"), this);
    QAction* newSoil = new QAction(tr("&New Soil"), this);
    QAction* deleteSoil = new QAction(tr("&Delete Soil"), this);
    restoreData = new QAction(tr("&Restore Data"), this);
    addHorizon = new QAction(tr("&Add Horizon"), this);
    deleteHorizon = new QAction(tr("&Delete Horizon"), this);
    addHorizon->setEnabled(false);
    deleteHorizon->setEnabled(false);
    restoreData->setEnabled(false);
    saveChanges->setEnabled(false);

    useWaterRetentionData = new QAction(tr("&Use Water Retention Data"), this);
    airEntryFixed = new QAction(tr("&Air Entry fixed"), this);
    parameterRestriction = new QAction(tr("&Parameters Restriction (m=1-1/n)"), this);

    useWaterRetentionData->setCheckable(true);
    airEntryFixed->setCheckable(true);
    airEntryFixed->setEnabled(false);
    parameterRestriction->setCheckable(true);
    parameterRestriction->setEnabled(false);
    useWaterRetentionData->setChecked(fittingOptions.useWaterRetentionData);
    airEntryFixed->setChecked(fittingOptions.airEntryFixed);
    parameterRestriction->setChecked(fittingOptions.mRestriction);

    connect(openSoilDB, &QAction::triggered, this, &Crit3DSoilWidget::on_actionOpenSoilDB);
    connect(saveChanges, &QAction::triggered, this, &Crit3DSoilWidget::on_actionSave);
    connect(newSoil, &QAction::triggered, this, &Crit3DSoilWidget::on_actionNewSoil);
    connect(deleteSoil, &QAction::triggered, this, &Crit3DSoilWidget::on_actionDeleteSoil);
    connect(restoreData, &QAction::triggered, this, &Crit3DSoilWidget::on_actionRestoreData);
    connect(addHorizon, &QAction::triggered, this, &Crit3DSoilWidget::on_actionAddHorizon);
    connect(deleteHorizon, &QAction::triggered, this, &Crit3DSoilWidget::on_actionDeleteHorizon);

    connect(useWaterRetentionData, &QAction::triggered, this, &Crit3DSoilWidget::on_actionUseWaterRetentionData);
    connect(airEntryFixed, &QAction::triggered, this, &Crit3DSoilWidget::on_actionAirEntry);
    connect(parameterRestriction, &QAction::triggered, this, &Crit3DSoilWidget::on_actionAirEntry);

    connect(&soilListComboBox, &QComboBox::currentTextChanged, this, &Crit3DSoilWidget::on_actionChooseSoil);
    connect(horizonsTab, SIGNAL(horizonSelected(int)), this, SLOT(setInfoTextural(int)));
    connect(wrCurveTab, SIGNAL(horizonSelected(int)), this, SLOT(setInfoTextural(int)));
    connect(tabWidget, &QTabWidget::currentChanged, [=](int index){ this->tabChanged(index); });

    fileMenu->addAction(openSoilDB);
    fileMenu->addAction(saveChanges);
    editMenu->addAction(newSoil);
    editMenu->addAction(deleteSoil);
    editMenu->addAction(restoreData);
    editMenu->addAction(addHorizon);
    editMenu->addAction(deleteHorizon);
    fittingMenu->addAction(useWaterRetentionData);
    fittingMenu->addAction(airEntryFixed);
    fittingMenu->addAction(parameterRestriction);

}


void Crit3DSoilWidget::setDbSoil(QString dbSoilName, QString soilCode)
{
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
    soilListComboBox.clear();
    for (int i = 0; i < soilStringList.size(); i++)
    {
        soilListComboBox.addItem(soilStringList[i]);
    }

    soilListComboBox.setCurrentText(soilCode);
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
    saveChanges->setEnabled(true);
}


void Crit3DSoilWidget::on_actionChooseSoil(QString soilCode)
{
    // re load textural triangle to clean previous circle
    pic.load(picPath);
    labelPic->setPixmap(pic);
    // soilListComboBox has been cleared
    if (soilCode.isEmpty())
    {
        return;
    }
    horizonsTab->setInsertSoilElement(false);
    wrDataTab->setFillData(false);
    wrCurveTab->setFillElement(false);
    QString error;
    // somethig has been modified, ask for saving
    if (horizonsTab->getSoilCodeChanged() == true || wrDataTab->getSoilCodeChanged() == true)
    {

        QString soilCodeChanged = QString::fromStdString(mySoil.code);
        QMessageBox::StandardButton confirm;
        QString msg = "Do you want to save changes to soil "+ soilCodeChanged + " ?";
        confirm = QMessageBox::question(nullptr, "Warning", msg, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

        if (confirm == QMessageBox::Yes)
        {
            if (horizonsTab->getSoilCodeChanged() == true)
            {
                if (!updateSoilData(&dbSoil, soilCodeChanged, &mySoil, &error))
                {
                    QMessageBox::critical(nullptr, "Error!", error);
                    return;
                }
            }
            if (wrDataTab->getSoilCodeChanged() == true)
            {
                // update Soil
                wrDataTab->updateSoil(&mySoil);
                QVector<int> horizonChanged = wrDataTab->getHorizonChanged();
                // update water_retention DB table
                for (int i = 0; i < horizonChanged.size(); i++)
                {
                    if (!updateWaterRetentionData(&dbSoil, soilCodeChanged, &mySoil, horizonChanged[i], &error))
                    {
                        QMessageBox::critical(nullptr, "Error!", error);
                        return;
                    }

                }

            }
        }
        horizonsTab->resetSoilCodeChanged();
        wrDataTab->resetSoilCodeChanged();
    }


    if (! loadSoil(&dbSoil, soilCode, &mySoil, textureClassList, &fittingOptions, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }
    savedSoil = mySoil;

    soilName = QString::fromStdString(mySoil.name);
    satValue->clear();
    fcValue->clear();
    wpValue->clear();
    awValue->clear();
    potFCValue->clear();

    restoreData->setEnabled(true);
    infoGroup->setVisible(true);
    infoGroup->setTitle(soilName);
    soilCodeValue->setText(QString::fromStdString(mySoil.code));

    tabChanged(tabWidget->currentIndex());
    // circle inside triangle
    for (unsigned int i = 0; i < mySoil.nrHorizons; i++)
    {
        {
            if (soil::getUSDATextureClass(mySoil.horizon[i].dbData.sand, mySoil.horizon[i].dbData.silt, mySoil.horizon[i].dbData.clay) != NODATA)
            {
                // the pic has white space around the triangle: widthTriangle and heightTriangle define triangle size without white space
                double widthOffset = (pic.width() - widthTriangle)/2;
                double heightOffset = (pic.height() - heightTriangle)/2;
                double factor = ( pow ( (pow(100.0, 2.0) - pow(50.0, 2.0)), 0.5) ) / 100;
                // draw new point
                double cx = widthTriangle * ((mySoil.horizon[i].dbData.silt + mySoil.horizon[i].dbData.clay / 2) / 100);
                double cy =  heightTriangle * (1 - mySoil.horizon[i].dbData.clay  / 2 * pow (3, 0.5) / 100 / factor); // tg(60°)=3^0.5
                painter.begin(&pic);
                QPen pen(Qt::red);
                painter.setPen(pen);

                QPointF center(widthOffset + cx, heightOffset + cy);
                painter.setBrush(Qt::transparent);
                painter.drawEllipse(center,4.5,4.5);

                painter.end();
                labelPic->setPixmap(pic);
            }
        }
    }
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
        QString error;

        if (confirm == QMessageBox::Yes)
        {
            if (deleteSoilData(&dbSoil, soilListComboBox.currentText(), &error))
            {
                soilListComboBox.removeItem(soilListComboBox.currentIndex());
                horizonsTab->resetSoilCodeChanged();
                wrDataTab->resetSoilCodeChanged();
            }
        }
        else
        {
            return;
        }
    }
}


void Crit3DSoilWidget::on_actionUseWaterRetentionData()
{
    if (useWaterRetentionData->isChecked())
    {
        airEntryFixed->setEnabled(true);
        parameterRestriction->setEnabled(true);
    }
    else
    {
        airEntryFixed->setEnabled(false);
        parameterRestriction->setEnabled(false);
    }

    fittingOptions.useWaterRetentionData = this->useWaterRetentionData->isChecked();
    // TO DO: update
}


void Crit3DSoilWidget::on_actionAirEntry()
{
    fittingOptions.airEntryFixed = this->airEntryFixed->isChecked();
    // TO DO: update
}


void Crit3DSoilWidget::on_actionParameterRestriction()
{
    fittingOptions.mRestriction = this->parameterRestriction->isChecked();
    // TO DO: update
}


void Crit3DSoilWidget::on_actionSave()
{
    QString error;
    if (horizonsTab->getSoilCodeChanged() == false && wrDataTab->getSoilCodeChanged() == false)
    {
        QMessageBox::critical(nullptr, "Warning", "The soil has already been updated");
        return;
    }
    QString soilCodeChanged = QString::fromStdString(mySoil.code);
    if (horizonsTab->getSoilCodeChanged() == true)
    {
        if (!updateSoilData(&dbSoil, soilCodeChanged, &mySoil, &error))
        {
            QMessageBox::critical(nullptr, "Error!", error);
            return;
        }
        horizonsTab->resetSoilCodeChanged();
    }
    if (wrDataTab->getSoilCodeChanged() == true)
    {
        // update Soil
        wrDataTab->updateSoil(&mySoil);
        QVector<int> horizonChanged = wrDataTab->getHorizonChanged();
        // update water_retention DB table
        for (int i = 0; i < horizonChanged.size(); i++)
        {
            if (!updateWaterRetentionData(&dbSoil, soilCodeChanged, &mySoil, horizonChanged[i], &error))
            {
                QMessageBox::critical(nullptr, "Error!", error);
                return;
            }

        }
        wrDataTab->resetSoilCodeChanged();
    }
    savedSoil = mySoil;
}

void Crit3DSoilWidget::on_actionRestoreData()
{
    mySoil = savedSoil;
    horizonsTab->setInsertSoilElement(false);
    wrDataTab->setFillData(false);
    wrCurveTab->setFillElement(false);

    tabChanged(tabWidget->currentIndex());
}

void Crit3DSoilWidget::on_actionAddHorizon()
{
    horizonsTab->addRowClicked();
}

void Crit3DSoilWidget::on_actionDeleteHorizon()
{
    horizonsTab->removeRowClicked();
}

void Crit3DSoilWidget::setInfoTextural(int nHorizon)
{
    // re load textural triangle to clean previous circle
    pic.load(picPath);
    labelPic->setPixmap(pic);
    for (unsigned int i = 0; i < mySoil.nrHorizons; i++)
    {
        if (soil::getUSDATextureClass(mySoil.horizon[i].dbData.sand, mySoil.horizon[i].dbData.silt, mySoil.horizon[i].dbData.clay) != NODATA)
        {
            // the pic has white space around the triangle: widthTriangle and heightTriangle define triangle size without white space
            double widthOffset = (pic.width() - widthTriangle)/2;
            double heightOffset = (pic.height() - heightTriangle)/2;
            double factor = ( pow ( (pow(100.0, 2.0) - pow(50.0, 2.0)), 0.5) ) / 100;
            // draw new point
            double cx = widthTriangle * ((mySoil.horizon[i].dbData.silt + mySoil.horizon[i].dbData.clay / 2) / 100);
            double cy =  heightTriangle * (1 - mySoil.horizon[i].dbData.clay  / 2 * pow (3, 0.5) / 100 / factor); // tg(60°)=3^0.5
            painter.begin(&pic);
            QPen pen(Qt::red);
            painter.setPen(pen);

            QPointF center(widthOffset + cx, heightOffset + cy);
            if (i == nHorizon)
            {
                painter.setBrush(Qt::red);
                painter.drawEllipse(center,4.5,4.5);
            }
            else
            {
                painter.setBrush(Qt::transparent);
                painter.drawEllipse(center,4.5,4.5);
            }

            painter.end();
            labelPic->setPixmap(pic);
        }
    }

    // nHorizon = -1 : nothing is selected, clear all
    if (nHorizon == -1)
    {
        satValue->setText("");
        fcValue->setText("");
        wpValue->setText("");
        awValue->setText("");
        potFCValue->setText("");
    }
    else
    {
        if (mySoil.horizon[nHorizon].vanGenuchten.thetaS == NODATA)
        {
            satValue->setText(QString::number(NODATA));
        }
        else
        {
            satValue->setText(QString::number(mySoil.horizon[nHorizon].vanGenuchten.thetaS, 'f', 3));
        }

        if (mySoil.horizon[nHorizon].waterContentFC == NODATA)
        {
            fcValue->setText(QString::number(NODATA));
        }
        else
        {
            fcValue->setText(QString::number(mySoil.horizon[nHorizon].waterContentFC, 'f', 3));
        }

        if (mySoil.horizon[nHorizon].waterContentWP == NODATA)
        {
            wpValue->setText(QString::number(NODATA));
        }
        else
        {
            wpValue->setText(QString::number(mySoil.horizon[nHorizon].waterContentWP, 'f', 3));
        }

        if (mySoil.horizon[nHorizon].waterContentFC == NODATA || mySoil.horizon[nHorizon].waterContentWP == NODATA)
        {
            awValue->setText(QString::number(NODATA));
        }
        else
        {
            awValue->setText(QString::number(mySoil.horizon[nHorizon].waterContentFC-mySoil.horizon[nHorizon].waterContentWP, 'f', 3));
        }

        if (mySoil.horizon[nHorizon].fieldCapacity == NODATA)
        {
            potFCValue->setText(QString::number(NODATA));
        }
        else
        {
            potFCValue->setText(QString::number(mySoil.horizon[nHorizon].fieldCapacity, 'f', 3));
        }
    }
}


void Crit3DSoilWidget::tabChanged(int index)
{

    if (index == 0)
    {
        if (!horizonsTab->getInsertSoilElement() && !soilListComboBox.currentText().isEmpty())
        {
            horizonsTab->insertSoilHorizons(&mySoil, textureClassList, &fittingOptions);
            addHorizon->setEnabled(true);
            deleteHorizon->setEnabled(true);
        }
    }
    if (index == 1) // tab water retention data
    {
        if (!wrDataTab->getFillData() && !soilListComboBox.currentText().isEmpty())
        {
            wrDataTab->insertData(&mySoil);
            addHorizon->setEnabled(false);
            deleteHorizon->setEnabled(false);
        }

    }
    else if (index == 2) // tab water retention curve
    {
        if (!wrCurveTab->getFillElement() && !soilListComboBox.currentText().isEmpty())
        {
            wrCurveTab->insertElements(&mySoil);
        }

    }

}

