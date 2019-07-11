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


Crit3DSoilWidget::Crit3DSoilWidget()
{
    this->setWindowTitle(QStringLiteral("Soil"));
    this->resize(800, 600);

    // layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(&soilListComboBox);
    layout->addWidget(&soilTextEdit);
    layout->setAlignment(Qt::AlignTop);
    this->setLayout(layout);

    // menu
    QMenuBar* menuBar = new QMenuBar();
    QMenu *fileMenu = new QMenu("File");
    menuBar->addMenu(fileMenu);

    QAction* OpenSoilDB = new QAction(tr("&Open soil database"), this);
    connect(OpenSoilDB, &QAction::triggered, this, &Crit3DSoilWidget::on_actionOpenSoilDB);
    fileMenu->addAction(OpenSoilDB);

    this->layout()->setMenuBar(menuBar);

    connect(&soilListComboBox, &QComboBox::currentTextChanged, this, &Crit3DSoilWidget::on_actionChooseSoil);
}


void Crit3DSoilWidget::on_actionOpenSoilDB()
{
    QString dbSoilName = QFileDialog::getOpenFileName(this, tr("Load soil data"), "", tr("SQLite files (*.db)"));
    if (dbSoilName == "") return;

    // open soil db
    QString error;
    if (! openDbSoil(dbSoilName, &dbSoil, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    // load VG parameters
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

    // show soil list with comboBox
    this->soilListComboBox.clear();
    for (int i = 0; i < soilList.size(); i++)
    {
        this->soilListComboBox.addItem(soilList[i]);
    }
}



void Crit3DSoilWidget::on_actionChooseSoil(QString soilCode)
{
    this->soilTextEdit.clear();
    this->soilTextEdit.append(soilCode);

    //if (! loadSoil)
}



void Crit3DSoilWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    //TODO
}

