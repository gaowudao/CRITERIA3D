#include "tabWaterRetentionCurve.h"

#include "commonConstants.h"
#include <qwt_point_data.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_event_pattern.h>
#include <qwt_picker_machine.h>
#include <QWidget>

#define XMIN 0.1
#define XMAX 100000
#define YMIN 0.0
#define YMAX 0.6

TabWaterRetentionCurve::TabWaterRetentionCurve()
{
    qDebug() << "TabWaterRetentionCurve constructor";
    // test
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *plotLayout = new QVBoxLayout;
    linesLayout = new QVBoxLayout;

    myPlot = new QwtPlot;
    myPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));
    myPlot->setAxisTitle(QwtPlot::yLeft,QString("Water content [%1]").arg(QString("m3 m-3")));
    myPlot->setAxisTitle(QwtPlot::xBottom,QString("Water potential [%1]").arg(QString("kPa")));
    myPlot->setAxisScale(QwtPlot::xBottom,XMIN, XMAX);
    myPlot->setAxisScale(QwtPlot::yLeft,YMIN, YMAX);

    // Setting Magnifier
    QwtPlotMagnifier* zoom_x = new QwtPlotMagnifier(myPlot->canvas());
    QwtPlotMagnifier* zoom_y = new QwtPlotMagnifier(myPlot->canvas());
    // Shift+MouseWheel --> Magnifier x
    zoom_x->setWheelModifiers(Qt::ShiftModifier);
    zoom_x->setAxisEnabled(QwtPlot::xBottom, true);
    zoom_x->setAxisEnabled(QwtPlot::yLeft, false);
    // CTRL + MouseWheel --> Magnifier y
    zoom_y->setWheelModifiers(Qt::ControlModifier);
    zoom_y->setAxisEnabled(QwtPlot::xBottom,false);
    zoom_y->setAxisEnabled(QwtPlot::yLeft,true);

    // Left Button for panning
    QwtPlotPanner* panner = new QwtPlotPanner (myPlot->canvas());
    panner->setMouseButton(Qt::LeftButton);
    QwtPlotZoomer* zoomer = new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, myPlot->canvas()  );
    zoomer->setRubberBandPen( QColor( Qt::black ) );
    zoomer->setTrackerPen( QColor( Qt::red ) );
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

}

void TabWaterRetentionCurve::insertElements(soil::Crit3DSoil *soil)
{

    QRect layoutSize = linesLayout->geometry();

    int x1 = layoutSize.topLeft().x();
    int totHeight = layoutSize.height();

    if (soil!=nullptr)
    {
        mySoil = soil;
        // delete all Widgets
        if (!lineList.isEmpty())
        {
            qDebug() << "delete all Widgets";
            qDeleteAll(lineList);
            lineList.clear();

            qDeleteAll(curveList);
            curveList.clear();
        }
        QVector<double> xVector;
        QVector<double> yVector;
        double x;

        for (int i = 0; i<mySoil->nrHorizons; i++)
        {

            // insertVerticalLines
            int length = (mySoil->horizon[i].lowerDepth*100 - mySoil->horizon[i].upperDepth*100) * totHeight / (mySoil->totalDepth*100);
            LineHorizont* line = new LineHorizont();
            line->setIndex(i);
            line->setFixedWidth(20);
            line->setFixedHeight(length);
            line->setClass(mySoil->horizon[i].texture.classUSDA);
            linesLayout->addWidget(line);
            lineList.push_back(line);

            // insert Curves
            QwtPlotCurve *curve = new QwtPlotCurve;
            xVector.clear();
            yVector.clear();
            x = XMIN;
            while (x < XMAX)
            {
                double y = soil::thetaFromSignPsi(-x, &mySoil->horizon[i]);
                if (y != NODATA)
                {
                    xVector.push_back(x);
                    yVector.push_back(y);
                }
                x=x*1.25;
            }
            QwtPointArrayData *data = new QwtPointArrayData(xVector,yVector);
            curve->setSamples(data);
            curve->attach(myPlot);
            curveList.push_back(curve);
        }
        pick = new MyPicker(myPlot, curveList);
        pick->setStateMachine(new QwtPickerClickPointMachine());

        for (int i=0; i<lineList.size(); i++)
        {
            connect(lineList[i], SIGNAL(clicked(int)), this, SLOT(selectionElement(int)));
        }
        linesLayout->update();
        myPlot->replot();
    }

}

void TabWaterRetentionCurve::selectionElement(int index)
{

    // check selection state
    if (lineList[index]->getSelected())
    {
        // clear previous selection
        for(int i = 0; i < lineList.size(); i++)
        {
            if (i != index)
            {
                lineList[i]->restorePalette();
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

}
