#include "graphs.h"
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>


class DistancePicker: public QwtPlotPicker
{
public:
    DistancePicker( QWidget *canvas ):
        QwtPlotPicker( canvas )
    {
        setTrackerMode( QwtPicker::ActiveOnly );
        setStateMachine( new QwtPickerDragLineMachine() );
        setRubberBand( QwtPlotPicker::PolygonRubberBand );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QwtText text;

        const QPolygon points = selection();
        if ( !points.isEmpty() )
        {
            QString num;
            num.setNum( QLineF( pos, invTransform( points[0] ) ).length() );

            QColor bg( Qt::white );
            bg.setAlpha( 200 );

            text.setBackgroundBrush( QBrush( bg ) );
            text.setText( num );
        }
        return text;
    }
};

Plot::Plot( QWidget *parent ):
    QwtPlot( parent )
{
    canvas()->setStyleSheet(
        "border: 1px solid Black;"
        "border-radius: 15px;"
        "background-color: white;"
    );

    // panning with the left mouse button
    (void )new QwtPlotPanner( canvas() );

    // zoom in/out with the wheel
    QwtPlotMagnifier *magnifier = new QwtPlotMagnifier( canvas() );
    magnifier->setMouseButton( Qt::NoButton );

    // distanve measurement with the right mouse button
    DistancePicker *picker = new DistancePicker( canvas() );
    picker->setMousePattern( QwtPlotPicker::MouseSelect1, Qt::RightButton );
    picker->setRubberBandPen( QPen( Qt::blue ) );
}

void Plot::addCurve(QString myTitle, QwtPlotCurve::CurveStyle myStyle, QwtSymbol* mySymbol, QVector<QPointF> &samples)
{
    QwtPlotCurve* myCurve = new QwtPlotCurve(myTitle);

    myCurve->setPen( QColor( "Purple" ) );
    myCurve->setStyle(myStyle);
    myCurve->setSymbol(mySymbol);
    myCurve->setSamples(samples);
    myCurve->attach( this );
}

QVector<QPointF> getProfileSeries(heat_output* myOut, outputType myVar, int layerIndex)
{
    QVector<QPointF> mySeries;

    switch (myVar)
    {
        case outputType::soilTemperature :
            for (int i=0; i<myOut->nrValues; i++)
            {
                mySeries.push_back(myOut->profileOutput[i].temperature[layerIndex]);
            }
        break;

        case outputType::soilWater :
            for (int i=0; i<myOut->nrValues; i++)
            {
                mySeries.push_back(myOut->profileOutput[i].waterContent[layerIndex]);
            }
        break;
    }

    return mySeries;
}

void Plot::drawProfile(outputType graphType, heat_output* myOut)
{
    QVector<QPointF> mySeries;

    switch (graphType)
    {
        case outputType::soilTemperature :

        for (int z=0; z<myOut->nrLayers; z++)
        {
            mySeries = getProfileSeries(myOut, graphType, z);
            addCurve("soil temperature", QwtPlotCurve::Lines, NULL, mySeries);
        }
        break;

        for (int z=0; z<myOut->nrLayers; z++)
        {
            mySeries = getProfileSeries(myOut, graphType, z);
            addCurve("soil water content", QwtPlotCurve::Lines, NULL, mySeries);
        }
        break;

    }
}








