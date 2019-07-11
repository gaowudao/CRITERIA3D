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
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>


Crit3DSoilWidget::Crit3DSoilWidget()
{
    this->setWindowTitle(QStringLiteral("Soil"));
    this->resize(800, 600);

    // layout
    soilListCombo = new QComboBox();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(soilListCombo);
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
}


void Crit3DSoilWidget::on_actionOpenSoilDB()
{
    QString dbSoilName = QFileDialog::getOpenFileName(this, tr("Load soil data"), "", tr("SQLite files (*.db)"));
    if (dbSoilName == "") return;

    QString error;
    QSqlDatabase* dbSoil = new QSqlDatabase();
    if (! openDbSoil(dbSoilName, dbSoil, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    if (! loadVanGenuchtenParameters(dbSoil, this->soilClassList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    this->soilListCombo->clear();
    QStringList soilList;

    if (! getSoilList(dbSoil, &soilList, &error))
    {
        QMessageBox::critical(nullptr, "Error!", error);
        return;
    }

    for (int i = 0; i < soilList.size(); i++)
    {
        this->soilListCombo->addItem(soilList[i]);
    }

    if(error != "")
    {
        QMessageBox::information(nullptr, "Warning", error);
    }

    dbSoil->close();
}



void Crit3DSoilWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev);

    //TODO
}

