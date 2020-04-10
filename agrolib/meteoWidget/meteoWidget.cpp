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


#include "meteoWidget.h"
#include "dialogSelectVar.h"
#include "utilities.h"
#include "commonConstants.h"
#include "formInfo.h"

#include <QMessageBox>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QDate>

#include <QDebug>


Crit3DMeteoWidget::Crit3DMeteoWidget()
{
    this->setWindowTitle(QStringLiteral("Graph"));
    this->resize(1240, 700);
    currentFreq = daily; //default
    QVector<QLineSeries*> vectorLine;
    QVector<QBarSet*> vectorBarSet;

    QString csvPath, defaultPath, stylesPath;
    if (searchDataPath(&csvPath))
    {
        defaultPath = csvPath + "SETTINGS/Crit3DPlotDefault.csv";
        stylesPath = csvPath + "SETTINGS/Crit3DPlotStyles.csv";
    }

    // read Crit3DPlotDefault and fill MapCSVDefault
    int CSVRequiredInfo = 3;
    QFile fileDefaultGraph(defaultPath);
    if ( !fileDefaultGraph.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else
    {
        QTextStream in(&fileDefaultGraph);
        in.readLine(); //skip first line
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList items = line.split(",");
            if (items.size() < CSVRequiredInfo)
            {
                qDebug() << "invalid format CSV, missing data";
            }
            QString key = items[0];
            items.removeFirst();
            if (key.isEmpty() || items[0].isEmpty())
            {
                qDebug() << "invalid format CSV, missing data";
            }
            if (key.contains("DAILY"))
            {
                currentFreq = daily;
            }
            else
            {
                currentFreq = hourly;
            }
            MapCSVDefault.insert(key,items);
            if (items[0] == "line")
            {
                QLineSeries* line = new QLineSeries();
                line->setName(key);
                line->setColor(QColor(items[1]));
                vectorLine.append(line);
                currentVariables.append(key);
            }
            if (items[0] == "bar")
            {
                QBarSet* set = new QBarSet(key);
                set->setColor(QColor(items[1]));
                set->setBorderColor(QColor(items[1]));
                vectorBarSet.append(set);
                currentVariables.append(key);
            }
        }
    }
    lineSeries.append(vectorLine);
    setVector.append(vectorBarSet);

    // read Crit3DPlotStyles and fill MapCSVStyles
    QFile fileStylesGraph(stylesPath);
    if ( !fileStylesGraph.open(QFile::ReadOnly | QFile::Text) ) {
        qDebug() << "File not exists";
    }
    else
    {
        QTextStream in(&fileStylesGraph);
        in.readLine(); //skip first line
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList items = line.split(",");
            if (items.size() < CSVRequiredInfo)
            {
                qDebug() << "invalid format CSV, missing data";
            }
            QString key = items[0];
            items.removeFirst();
            if (key.isEmpty() || items[0].isEmpty())
            {
                qDebug() << "invalid format CSV, missing data";
            }
            MapCSVStyles.insert(key,items);
        }
    }

    // layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QGroupBox *horizontalGroupBox = new QGroupBox();
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QVBoxLayout *plotLayout = new QVBoxLayout;

    dailyButton = new QPushButton(tr("daily"));
    hourlyButton = new QPushButton(tr("hourly"));
    addVarButton = new QPushButton(tr("+/- var"));
    dailyButton->setMaximumWidth(this->width()/8);
    hourlyButton->setMaximumWidth(this->width()/8);
    addVarButton->setMaximumWidth(this->width()/8);

    if (currentFreq == daily)
    {
        dailyButton->setEnabled(false);
        hourlyButton->setEnabled(true);
    }
    else if(currentFreq == hourly)
    {
        hourlyButton->setEnabled(false);
        dailyButton->setEnabled(true);
    }

    buttonLayout->addWidget(dailyButton);
    buttonLayout->addWidget(hourlyButton);
    buttonLayout->addWidget(addVarButton);
    buttonLayout->setAlignment(Qt::AlignLeft);
    chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setChart(chart);

    axisX = new QBarCategoryAxis();
    axisXvirtual = new QDateTimeAxis();
    axisY = new QValueAxis();
    axisYdx = new QValueAxis();

    QDate first(QDate::currentDate().year(), 1, 1);
    QDate last(QDate::currentDate().year(), 12, 31);
    axisX->setTitleText("Date");
    axisX->setGridLineVisible(false);
    axisXvirtual->setTitleText("Date");
    axisXvirtual->setFormat("MMM dd <br> yyyy");
    axisXvirtual->setMin(QDateTime(first, QTime(0,0,0)));
    axisXvirtual->setMax(QDateTime(last, QTime(0,0,0)));
    axisXvirtual->setTickCount(13);

    axisY->setRange(0,30);
    axisY->setGridLineVisible(false);

    axisYdx->setRange(0,8);
    axisYdx->setGridLineVisible(false);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisXvirtual, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->addAxis(axisYdx, Qt::AlignRight);

    QBarSeries* barFirstSeries = new QBarSeries();
    for (int i = 0; i < setVector[0].size(); i++)
    {
        barFirstSeries->append(setVector[0][i]);
    }
    barSeries.append(barFirstSeries);
    if (setVector[0].size() != 0)
    {
        chart->addSeries(barSeries[0]);
        barSeries[0]->attachAxis(axisX);
        barSeries[0]->attachAxis(axisYdx);
    }

    for (int i = 0; i < lineSeries[0].size(); i++)
    {
        chart->addSeries(lineSeries[0][i]);
        lineSeries[0][i]->attachAxis(axisX);
        lineSeries[0][i]->attachAxis(axisY);
    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chartView->setRenderHint(QPainter::Antialiasing);
    axisX->hide();

    connect(addVarButton, &QPushButton::clicked, [=](){ showVar(); });
    connect(dailyButton, &QPushButton::clicked, [=](){ showDailyGraph(); });
    connect(hourlyButton, &QPushButton::clicked, [=](){ showHourlyGraph(); });

    plotLayout->addWidget(chartView);
    horizontalGroupBox->setLayout(buttonLayout);
    mainLayout->addWidget(horizontalGroupBox);
    mainLayout->addLayout(plotLayout);
    setLayout(mainLayout);
}

void Crit3DMeteoWidget::draw(Crit3DMeteoPoint mpVector)
{
    meteoPoints.append(mpVector);
    resetValues();
    if (currentFreq == daily)
    {
        drawDailyVar();
    }
    else if (currentFreq == hourly)
    {
        drawHourlyVar();
    }
}

void Crit3DMeteoWidget::resetValues()
{

    QVector<QLineSeries*> vectorLine;
    for (int i = 0; i<lineSeries[0].size(); i++)
    {
        QLineSeries* line = new QLineSeries();
        line->setName(lineSeries[0][i]->name());
        line->setColor(lineSeries[0][i]->color());
        vectorLine.append(line);
    }

    QStringList nameBar;
    QVector<QColor> colorBar;
    int sizeBarSet = setVector[0].size();

    for (int i = 0; i < sizeBarSet; i++)
    {
        nameBar.append(setVector[0][i]->label());
        colorBar.append(setVector[0][i]->color());
    }

    // clear prev series values
    for (int mp=0; mp<meteoPoints.size()-1;mp++)
    {
        for (int i = 0; i < lineSeries[mp].size(); i++)
        {
            lineSeries[mp][i]->clear();
        }
        lineSeries[mp].clear();
        setVector[mp].clear();
        barSeries[mp]->clear();
    }
    barSeries.clear();
    setVector.clear();
    lineSeries.clear();
    chart->removeAllSeries();

    // add lineSeries elements for each mp, clone lineSeries[0]
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        lineSeries.append(vectorLine);
    }

    QVector<QBarSet*> vectorBarSet;
    for (int i = 0; i < sizeBarSet; i++)
    {
        QBarSet* set = new QBarSet(nameBar[i]);
        set->setColor(colorBar[i]);
        set->setBorderColor(colorBar[i]);
        vectorBarSet.append(set);
    }

    // add vectorBarSet elements for each mp
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        setVector.append(vectorBarSet);
    }

    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        for (int i = 0; i < lineSeries[mp].size(); i++)
        {
            chart->addSeries(lineSeries[mp][i]);
            lineSeries[mp][i]->attachAxis(axisX);
            lineSeries[mp][i]->attachAxis(axisY);
        }
    }
}

void Crit3DMeteoWidget::drawDailyVar()
{

    FormInfo formInfo;

    Crit3DDate firstDate;
    Crit3DDate myDate;
    long nDays = 0;
    double maxBar = 0;
    double maxLine = NODATA;

    nDays = meteoPoints[0].nrObsDataDaysD;
    firstDate = meteoPoints[0].obsDataD[0].date;

    int step = formInfo.start("Compute model...", nDays);
    int cont = 0;
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        for (int day = 0; day<nDays; day++)
        {
            if ( (cont % step) == 0) formInfo.setValue(cont);
            myDate = firstDate.addDays(day);
            categories.append(QString::number(day+1));
            for (int i = 0; i < lineSeries[mp].size(); i++)
            {
                meteoVariable meteoVar = MapDailyMeteoVar.at(lineSeries[mp][i]->name().toStdString());
                double value = meteoPoints[mp].getMeteoPointValueD(myDate, meteoVar);
                lineSeries[mp][i]->append(day+1, value);
                if (value > maxLine)
                {
                    maxLine = value;
                }
            }
            for (int j = 0; j < setVector[mp].size(); j++)
            {
                meteoVariable meteoVar = MapDailyMeteoVar.at(setVector[mp][j]->label().toStdString());
                double value = meteoPoints[mp].getMeteoPointValueD(myDate, meteoVar);
                *setVector[mp][j] << value;
                if (value > maxBar)
                {
                    maxBar = value;
                }
            }
            cont++; // formInfo update
        }
    }
    formInfo.close();

    QBarSeries* barMpSeries = new QBarSeries();
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        for (int i = 0; i < setVector[mp].size(); i++)
        {
            barMpSeries->append(setVector[mp][i]);
        }
        barSeries.append(barMpSeries);
    }

    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        if (setVector[mp].size() != 0)
        {
            chart->addSeries(barSeries[mp]);
            barSeries[mp]->attachAxis(axisX);
            barSeries[mp]->attachAxis(axisYdx);
        }
    }
    axisX->append(categories);
    axisX->setGridLineVisible(false);
    // update virtual x axis
    QDate first(firstDate.year, firstDate.month, firstDate.day);
    QDate last = first.addDays(nDays);
    axisXvirtual->setFormat("MMM dd <br> yyyy");
    axisXvirtual->setTickCount(13);
    axisXvirtual->setMin(QDateTime(first, QTime(0,0,0)));
    axisXvirtual->setMax(QDateTime(last, QTime(0,0,0)));
    axisYdx->setRange(0,maxBar);
    axisY->setMax(maxLine);
}

void Crit3DMeteoWidget::drawHourlyVar()
{

    FormInfo formInfo;

    Crit3DDate firstDate;
    Crit3DDate myDate;
    long nDays = 0;
    int nValues = 0;
    double maxBar = 0;
    double maxLine = NODATA;

    nDays = meteoPoints[0].nrObsDataDaysH;
    firstDate = meteoPoints[0].getMeteoPointHourlyValuesDate(0);

    int step = formInfo.start("Compute model...", nDays*24);
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        for (int day = 0; day<nDays; day++)
        {
            myDate = firstDate.addDays(day);
            for (int hour = 1; hour < 25; hour++)
            {
                if ( (nValues % step) == 0) formInfo.setValue(nValues);
                nValues = nValues + 1;
                categories.append(QString::number(nValues));
                for (int i = 0; i < lineSeries[mp].size(); i++)
                {
                    meteoVariable meteoVar = MapHourlyMeteoVar.at(lineSeries[mp][i]->name().toStdString());
                    double value = meteoPoints[mp].getMeteoPointValueH(myDate, hour, 0, meteoVar);
                    lineSeries[mp][i]->append(nValues, value);
                    if (value > maxLine)
                    {
                        maxLine = value;
                    }
                }
                for (int j = 0; j < setVector[mp].size(); j++)
                {
                    meteoVariable meteoVar = MapHourlyMeteoVar.at(setVector[mp][j]->label().toStdString());
                    double value = meteoPoints[mp].getMeteoPointValueH(myDate, hour, 0, meteoVar);
                    *setVector[mp][j] << value;
                    if (value > maxBar)
                    {
                        maxBar = value;
                    }
                }
            }
        }
    }
    formInfo.close();

    QBarSeries* barMpSeries = new QBarSeries();
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        for (int i = 0; i < setVector[mp].size(); i++)
        {
            barMpSeries->append(setVector[mp][i]);
        }
        barSeries.append(barMpSeries);
    }

    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        if (setVector[mp].size() != 0)
        {
            chart->addSeries(barSeries[mp]);
            barSeries[mp]->attachAxis(axisX);
            barSeries[mp]->attachAxis(axisYdx);
        }
    }
    axisX->append(categories);
    axisX->setGridLineVisible(false);
    // update virtual x axis
    QDate first(firstDate.year, firstDate.month, firstDate.day);
    QDate last = first.addDays(nDays);
    axisXvirtual->setFormat("MMM dd <br> yyyy <br> hh:mm");
    axisXvirtual->setTickCount(20); // TO DO how many?
    axisXvirtual->setMin(QDateTime(first, QTime(0,0,0)));
    axisXvirtual->setMax(QDateTime(last, QTime(0,0,0)));
    axisYdx->setRange(0,maxBar);
    axisY->setMax(maxLine);
}

void Crit3DMeteoWidget::showVar()
{
    QStringList allKeys = MapCSVStyles.keys();
    QStringList selectedVar = currentVariables;
    QStringList allVar;
    for (int i = 0; i<allKeys.size(); i++)
    {
        if (currentFreq == daily)
        {
            if (allKeys[i].contains("DAILY") && !selectedVar.contains(allKeys[i]))
            {
                allVar.append(allKeys[i]);
            }
        }
        else if (currentFreq == hourly)
        {
            if (!allKeys[i].contains("DAILY") && !selectedVar.contains(allKeys[i]))
            {
                allVar.append(allKeys[i]);
            }
        }
    }
    DialogSelectVar selectDialog(allVar, selectedVar);
    if (selectDialog.result() == QDialog::Accepted)
    {
        currentVariables.clear();
        currentVariables = selectDialog.getSelectedVariables();
        updateSeries();
        if (currentFreq == daily)
        {
            drawDailyVar();
        }
        else if (currentFreq == hourly)
        {
            drawHourlyVar();
        }
    }
}

void Crit3DMeteoWidget::showDailyGraph()
{
    currentFreq = daily;

    dailyButton->setEnabled(false);
    hourlyButton->setEnabled(true);

    for (int i = 0; i<currentVariables.size(); i++)
    {
        QString name = currentVariables[i];
        name = "DAILY_"+name;
        currentVariables[i] = name;
    }

    updateSeries();
    drawDailyVar();

}

void Crit3DMeteoWidget::showHourlyGraph()
{
    currentFreq = hourly;

    hourlyButton->setEnabled(false);
    dailyButton->setEnabled(true);

    for (int i = 0; i<currentVariables.size(); i++)
    {
        QString name = currentVariables[i];
        name = name.replace("DAILY_","");
        currentVariables[i] = name;
    }
    updateSeries();
    drawHourlyVar();
}

void Crit3DMeteoWidget::updateSeries()
{
    barSeries.clear();
    setVector.clear();
    lineSeries.clear();
    chart->removeAllSeries();

    QVector<QLineSeries*> vectorLine;
    QVector<QBarSet*> vectorBarSet;
    QMapIterator<QString, QStringList> i(MapCSVStyles);

    while (i.hasNext())
    {
        i.next();
        for (int j=0; j<currentVariables.size(); j++)
        {
            if (i.key() == currentVariables[j])
            {
                QStringList items = i.value();
                if (items[0] == "line")
                {
                    QLineSeries* line = new QLineSeries();
                    line->setName(i.key());
                    line->setColor(QColor(items[1]));
                    vectorLine.append(line);
                }
                if (items[0] == "bar")
                {
                    QBarSet* set = new QBarSet(i.key());
                    set->setColor(QColor(items[1]));
                    set->setBorderColor(QColor(items[1]));
                    vectorBarSet.append(set);
                }
            }
        }
    }

    // add lineSeries elements for each mp, clone lineSeries[0]
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        lineSeries.append(vectorLine);
    }

    // add vectorBarSet elements for each mp
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        setVector.append(vectorBarSet);
    }

    QBarSeries* barMpSeries = new QBarSeries();
    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        for (int i = 0; i < setVector[mp].size(); i++)
        {
            barMpSeries->append(setVector[mp][i]);
        }
        barSeries.append(barMpSeries);
    }

    for (int mp=0; mp<meteoPoints.size();mp++)
    {
        for (int i = 0; i < lineSeries[mp].size(); i++)
        {
            chart->addSeries(lineSeries[mp][i]);
            lineSeries[mp][i]->attachAxis(axisX);
            lineSeries[mp][i]->attachAxis(axisY);
        }
    }

}


