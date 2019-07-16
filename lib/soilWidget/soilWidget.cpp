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
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QDebug> //debug


Crit3DSoilWidget::Crit3DSoilWidget()
{
    this->setWindowTitle(QStringLiteral("Soil"));
    this->resize(500, 500);

    // layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(&soilListComboBox);
    layout->addWidget(&soilTextEdit);
    layout->setAlignment(Qt::AlignTop);
    this->setLayout(layout);

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

    useData = new QAction(tr("&Use Water Retention Data"), this);
    airEntry = new QAction(tr("&Air Entry fixed"), this);
    useData->setCheckable(true);
    airEntry->setCheckable(true);
    airEntry->setEnabled(false);

    connect(openSoilDB, &QAction::triggered, this, &Crit3DSoilWidget::on_actionOpenSoilDB);
    connect(saveChanges, &QAction::triggered, this, &Crit3DSoilWidget::on_actionSave);
    connect(newSoil, &QAction::triggered, this, &Crit3DSoilWidget::on_actionNewSoil);
    connect(deleteSoil, &QAction::triggered, this, &Crit3DSoilWidget::on_actionDeleteSoil);

    connect(useData, &QAction::triggered, this, &Crit3DSoilWidget::on_actionUseData);
    connect(airEntry, &QAction::triggered, this, &Crit3DSoilWidget::on_actionAirEntry);

    connect(&soilListComboBox, &QComboBox::currentTextChanged, this, &Crit3DSoilWidget::on_actionChooseSoil);

    fileMenu->addAction(openSoilDB);
    fileMenu->addAction(saveChanges);
    editMenu->addAction(newSoil);
    editMenu->addAction(deleteSoil);
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
    if (! loadVanGenuchtenParameters(&dbSoil, soilClassList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    // read soil list
    QStringList soilList;
    if (! getSoilList(&dbSoil, &soilList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    // show soil list
    this->soilListComboBox.clear();
    for (int i = 0; i < soilList.size(); i++)
    {
        this->soilListComboBox.addItem(soilList[i]);
    }
}


void Crit3DSoilWidget::on_actionChooseSoil(QString soilCode)
{
    this->soilTextEdit.clear();

    QString error;
    if (! loadSoil(&dbSoil, soilCode, &mySoil, soilClassList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    // show data (example)
    this->soilTextEdit.append(soilCode);
    this->soilTextEdit.append("Horizon nr., sand (%),   silt (%),   clay (%)");
    for (int i = 0; i < mySoil.nrHorizons; i++)
    {
        QString s;
        s = QString::number(mySoil.horizon[i].dbData.horizonNr)
                + "\t" + QString::number(mySoil.horizon[i].dbData.sand)
                + "\t" + QString::number(mySoil.horizon[i].dbData.silt)
                + "\t" + QString::number(mySoil.horizon[i].dbData.clay);
        this->soilTextEdit.append(s);
    }

    // warnings
    if (error != "")
    {
        QMessageBox::information(nullptr, "Warning", error);
    }
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
        airEntry->setChecked(false);
        // TO DO
    }
    else
    {
        airEntry->setChecked(false);
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
