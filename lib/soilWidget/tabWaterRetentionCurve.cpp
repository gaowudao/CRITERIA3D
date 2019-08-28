#include "tabWaterRetentionCurve.h"

#include "commonConstants.h"
#include "curvePanner.h"
#include <qwt_point_data.h>
#include <qwt_scale_engine.h>

#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_event_pattern.h>
#include <qwt_picker_machine.h>
#include <qwt_symbol.h>
#include <QWidget>

TabWaterRetentionCurve::TabWaterRetentionCurve()
{
    pick = nullptr;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *plotLayout = new QVBoxLayout;
    linesLayout = new QVBoxLayout;

    myPlot = new QwtPlot;
    myPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));
    myPlot->setAxisTitle(QwtPlot::yLeft,QString("Volumetric water content [%1]").arg(QString("m3 m-3")));
    myPlot->setAxisTitle(QwtPlot::xBottom,QString("Water potential [%1]").arg(QString("kPa")));
    myPlot->setAxisScale(QwtPlot::xBottom,XMIN, XMAX);
    myPlot->setAxisScale(QwtPlot::yLeft,YMIN, YMAX);

    // Left Button for panning
    Crit3DCurvePanner* panner = new Crit3DCurvePanner(myPlot);
    panner->setMouseButton(Qt::LeftButton);
    QwtPlotZoomer* zoomer = new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, myPlot->canvas()  );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::red ) );
    zoomer->setMaxStackDepth(5);
    // CTRL+LeftButton for the zooming
    zoomer->setMousePattern( QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ControlModifier);
    // CTRL+RightButton back to full size
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);

    // grid
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableY(true);
    grid->enableYMin(true);
    grid->setMajorPen( Qt::darkGray, 0, Qt::SolidLine );
    grid->setMinorPen( Qt::gray, 0 , Qt::DotLine );
    grid->attach(myPlot);
    mainLayout->addLayout(linesLayout);
    plotLayout->addWidget(myPlot);
    mainLayout->addLayout(plotLayout);

    setLayout(mainLayout);
    fillElement = false;
}

void TabWaterRetentionCurve::resetAll()
{
    // delete all Widgets
    if (!lineList.isEmpty())
    {
        qDeleteAll(lineList);
        lineList.clear();

        qDeleteAll(curveList);
        curveList.clear();
    }

    if (pick != nullptr)
    {
        delete pick;
        pick = nullptr;
    }

    if (!curveMarkerMap.isEmpty())
    {
        qDeleteAll(curveMarkerMap);
        curveMarkerMap.clear();
    }

    myPlot->detachItems( QwtPlotItem::Rtti_PlotCurve );
    fillElement = false;

}

bool TabWaterRetentionCurve::getFillElement() const
{
    return fillElement;
}

void TabWaterRetentionCurve::setFillElement(bool value)
{
    fillElement = value;
}

void TabWaterRetentionCurve::insertElements(soil::Crit3DSoil *soil)
{
    // rescale
    myPlot->setAxisScale(QwtPlot::xBottom, XMIN, XMAX);
    myPlot->setAxisScale(QwtPlot::yLeft, YMIN, YMAX);

    QRect layoutSize = linesLayout->geometry();

    int totHeight = 0;

    // if layoutSize has no size (case tab in use)
    if (layoutSize.height() == 0)
    {
        totHeight = this->height() - (this->height() * 5 / 100);
    }
    else
    {
        totHeight = layoutSize.height();
    }

    if (soil == nullptr) return;

    resetAll();
    fillElement = true;
    mySoil = soil;
    QVector<double> xVector;
    QVector<double> yVector;
    QVector<double> xMarkers;
    QVector<double> yMarkers;
    double x;
    double maxThetaSat = 0;

    for (unsigned int i = 0; i < mySoil->nrHorizons; i++)
    {
        // insertVerticalLines
        int length = int((mySoil->horizon[i].lowerDepth*100 - mySoil->horizon[i].upperDepth*100) * totHeight / (mySoil->totalDepth*100));
        BarHorizons* line = new BarHorizons();
        line->setIndex(signed(i));
        line->setFixedWidth(20);
        line->setFixedHeight(length);
        line->setClass(mySoil->horizon[i].texture.classUSDA);
        linesLayout->addWidget(line);
        lineList.push_back(line);

        // insert Curves
        QwtPlotCurve *curve = new QwtPlotCurve;
        xVector.clear();
        yVector.clear();
        double factor = 1.2;
        x = DXMIN;
        while (x < DXMAX*factor)
        {
            double y = soil::thetaFromSignPsi(-x, &mySoil->horizon[i]);
            if (y != NODATA)
            {
                xVector.push_back(x);
                yVector.push_back(y);
                maxThetaSat = MAXVALUE(maxThetaSat, y);
            }
            x *= factor;
        }
        QwtPointArrayData *data = new QwtPointArrayData(xVector,yVector);
        curve->setSamples(data);
        curve->attach(myPlot);
        curveList.push_back(curve);

        // insert marker
        if (!mySoil->horizon[i].dbData.waterRetention.empty())
        {
            QwtPlotCurve *curveMarkers = new QwtPlotCurve;
            xMarkers.clear();
            yMarkers.clear();
            for (unsigned int j = 0; j < mySoil->horizon[i].dbData.waterRetention.size(); j++)
            {
                curveMarkers->setSymbol(new QwtSymbol( QwtSymbol::Ellipse, QBrush( Qt::black ), QPen( Qt::black, 0 ), QSize( 5, 5 ) ));
                curveMarkers->setStyle(QwtPlotCurve::NoCurve);
                double x = mySoil->horizon[i].dbData.waterRetention[j].water_potential;
                double y = mySoil->horizon[i].dbData.waterRetention[j].water_content;
                if (x != NODATA && y != NODATA)
                {
                    xMarkers.push_back(x);
                    yMarkers.push_back(y);
                }
            }
            QwtPointArrayData *dataMarker = new QwtPointArrayData(xMarkers,yMarkers);
            curveMarkers->setSamples(dataMarker);
            curveMarkers->attach(myPlot);
            curveMarkerMap[i] = curveMarkers;
        }
    }

    // round maxThetaSat to first decimal
    maxThetaSat = ceil(maxThetaSat * 10) * 0.1;

    // rescale to maxThetaSat
    myPlot->setAxisScale(QwtPlot::yLeft, YMIN, std::max(YMAX, maxThetaSat));

    pick = new Crit3DCurvePicker(myPlot, curveList, curveMarkerMap);
    pick->setStateMachine(new QwtPickerClickPointMachine());
    connect(pick, SIGNAL(clicked(int)), this, SLOT(curveClicked(int)));

    for (int i=0; i<lineList.size(); i++)
    {
        connect(lineList[i], SIGNAL(clicked(int)), this, SLOT(widgetClicked(int)));
    }
    linesLayout->update();
    myPlot->replot();
}


void TabWaterRetentionCurve::widgetClicked(int index)
{
    // check selection state
    if (lineList[index]->getSelected())
    {
        // clear previous selection
        for(int i = 0; i < lineList.size(); i++)
        {
            if (i != index)
            {
                lineList[i]->restoreFrame();
                lineList[i]->setSelected(false);
            }
        }
        // select the right curve
        pick->setSelectedCurveIndex(index);
        pick->highlightCurve(true);
    }
    else
    {
        pick->highlightCurve(false);
        pick->setSelectedCurveIndex(-1);
    }
    emit horizonSelected(index);

}

void TabWaterRetentionCurve::curveClicked(int index)
{

    for(int i = 0; i < lineList.size(); i++)
    {
        if (i == index)
        {
            lineList[i]->setSelectedFrame();
            lineList[i]->setSelected(true);
        }
        else
        {
            lineList[i]->restoreFrame();
            lineList[i]->setSelected(false);
        }
    }
    emit horizonSelected(index);

}
