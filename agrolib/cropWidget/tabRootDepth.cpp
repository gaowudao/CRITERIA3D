#include "tabRootDepth.h"
#include "commonConstants.h"
#include "utilities.h"
#include <QMessageBox>


TabRootDepth::TabRootDepth()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *plotLayout = new QVBoxLayout;
    chart = new QChart();
    chartView = new QChartView(chart);
    chart->setTitle("Root Depth");
    chartView->setChart(chart);
    seriesRootDepth = new QLineSeries();
    seriesRootDepth->setName("rooth depth");
    seriesRootDepth->setColor(QColor(Qt::red));
    seriesRootDepthMin = new QLineSeries();
    seriesRootDepthMin->setName("root depht zero");
    seriesRootDepthMin->setColor(QColor(Qt::green));
    axisX = new QDateTimeAxis();
    axisY = new QValueAxis();

    chart->addSeries(seriesRootDepth);
    chart->addSeries(seriesRootDepthMin);
    QDate first(QDate::currentDate().year(), 1, 1);
    QDate last(QDate::currentDate().year(), 12, 31);
    axisX->setTitleText("Date");
    axisX->setFormat("MMM dd");
    axisX->setMin(QDateTime(first, QTime(0,0,0)));
    axisX->setMax(QDateTime(last, QTime(0,0,0)));
    axisX->setTickCount(13);
    chart->addAxis(axisX, Qt::AlignBottom);
    seriesRootDepth->attachAxis(axisX);
    seriesRootDepthMin->attachAxis(axisX);

    axisY->setTitleText("Depth  [m]");
    axisY->setReverse(true);
    axisY->setRange(0,2);
    axisY->setTickCount(5);
    chart->addAxis(axisY, Qt::AlignLeft);
    seriesRootDepth->attachAxis(axisY);
    seriesRootDepthMin->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chart->setAcceptHoverEvents(true);
    m_tooltip = new Callout(chart);
    connect(seriesRootDepthMin, &QLineSeries::hovered, this, &TabRootDepth::tooltipRDM);
    connect(seriesRootDepth, &QLineSeries::hovered, this, &TabRootDepth::tooltipRD);

    plotLayout->addWidget(chartView);
    mainLayout->addLayout(plotLayout);
    setLayout(mainLayout);
}

void TabRootDepth::computeRootDepth(Crit3DCrop* myCrop, Crit3DMeteoPoint *meteoPoint, int currentYear, const std::vector<soil::Crit3DLayer> &soilLayers)
{

    unsigned int nrLayers = unsigned(soilLayers.size());
    double totalSoilDepth = 0;
    if (nrLayers > 0) totalSoilDepth = soilLayers[nrLayers-1].depth + soilLayers[nrLayers-1].thickness / 2;

    year = currentYear;
    int prevYear = currentYear - 1;

    double waterTableDepth = NODATA;
    std::string error;

    Crit3DDate firstDate = Crit3DDate(1, 1, prevYear);
    Crit3DDate lastDate = Crit3DDate(31, 12, year);
    double tmin;
    double tmax;
    QDateTime x;

    seriesRootDepth->clear();
    seriesRootDepthMin->clear();

    int currentDoy = 1;
    myCrop->initialize(meteoPoint->latitude, nrLayers, totalSoilDepth, currentDoy);
    bool startValidData = false;

    for (Crit3DDate myDate = firstDate; myDate <= lastDate; ++myDate)
    {
        tmin = meteoPoint->getMeteoPointValueD(myDate, dailyAirTemperatureMin);
        tmax = meteoPoint->getMeteoPointValueD(myDate, dailyAirTemperatureMax);

        if (!myCrop->dailyUpdate(myDate, meteoPoint->latitude, soilLayers, tmin, tmax, waterTableDepth, &error))
        {
            QMessageBox::critical(nullptr, "Error!", QString::fromStdString(error));
            return;
        }

        // display only current year
        if (myDate.year == year)
        {
            x.setDate(QDate(myDate.year, myDate.month, myDate.day));
            if (myCrop->roots.rootDepthMin!= NODATA && myCrop->roots.rootDepth!= NODATA)
            {
                startValidData = true;
                seriesRootDepthMin->append(x.toMSecsSinceEpoch(), myCrop->roots.rootDepthMin);
                seriesRootDepth->append(x.toMSecsSinceEpoch(), myCrop->roots.rootDepth);
            }
            else if (startValidData && myCrop->roots.rootDepth == NODATA)
            {
                // last day
                seriesRootDepthMin->append(x.toMSecsSinceEpoch(), myCrop->roots.rootDepthMin);
                seriesRootDepth->append(x.toMSecsSinceEpoch(), myCrop->roots.rootDepthMin);
                startValidData = false;
            }
        }
    }

    // update x axis
    QDate first(year, 1, 1);
    QDate last(year, 12, 31);
    axisX->setMin(QDateTime(first, QTime(0,0,0)));
    axisX->setMax(QDateTime(last, QTime(0,0,0)));

}

void TabRootDepth::tooltipRDM(QPointF point, bool state)
{
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(chart);

    if (state)
    {
        QDateTime xDate;
        xDate.setMSecsSinceEpoch(point.x());
        m_tooltip->setText(QString("%1 \nroot ini %2 ").arg(xDate.date().toString("MMM dd")).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

void TabRootDepth::tooltipRD(QPointF point, bool state)
{
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(chart);

    if (state)
    {
        QDateTime xDate;
        xDate.setMSecsSinceEpoch(point.x());
        m_tooltip->setText(QString("%1 \nroot depth %2 ").arg(xDate.date().toString("MMM dd")).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}


