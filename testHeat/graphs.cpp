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
#include <qwt_plot_dict.h>

#include "gis.h"
#include "graphs.h"

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

    QwtLegend *legend = new QwtLegend;
    legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
    this->insertLegend(legend, QwtPlot::BottomLegend);
}

void Plot::addCurve(QString myTitle, QwtPlotCurve::CurveStyle myStyle, QPen myPen, QVector<QPointF> &samples)
{
    QwtPlotCurve* myCurve = new QwtPlotCurve(myTitle);

    myCurve->setPen(myPen);
    myCurve->setStyle(myStyle);
    myCurve->setSamples(samples);
    myCurve->attach( this );
}

QVector<QPointF> getProfileSeries(heat_output* myOut, outputType myVar, int layerIndex)
{
    QVector<QPointF> mySeries;
    QPointF myPoint;

    switch (myVar)
    {
        case outputType::soilTemperature :
            for (int i=0; i<myOut->nrValues; i++)
            {
                myPoint.setX(i);
                myPoint.setY(myOut->profileOutput[i].temperature[layerIndex].y());
                mySeries.push_back(myPoint);
            }
        break;

        case outputType::soilWater :
            for (int i=0; i<myOut->nrValues; i++)
            {
                myPoint.setX(i);
                myPoint.setY(myOut->profileOutput[i].waterContent[layerIndex].y());
                mySeries.push_back(myPoint);
            }
        break;
    }

    return mySeries;
}

void Plot::drawProfile(outputType graphType, heat_output* myOut)
{
    detachItems(QwtPlotItem::Rtti_PlotCurve, true);

    QPen myPen;

    gis::Crit3DColorScale myColorScale;
    myColorScale.nrKeyColors = 3;
    myColorScale.nrColors = 256;
    myColorScale.keyColor = new gis::Crit3DColor[myColorScale.nrKeyColors];
    myColorScale.color = new gis::Crit3DColor[myColorScale.nrColors];
    myColorScale.classification = classificationMethod::EqualInterval;
    myColorScale.keyColor[0] = gis::Crit3DColor(0, 0, 255);         /*!< blue */
    myColorScale.keyColor[1] = gis::Crit3DColor(255, 255, 0);       /*!< yellow */
    myColorScale.keyColor[2] = gis::Crit3DColor(255, 0, 0);         /*!< red */
    myColorScale.minimum = 0;
    myColorScale.maximum = myOut->nrLayers-1;
    myColorScale.classify();

    gis::Crit3DColor* myColor;
    QColor myQColor;
    QString myTitle;

    float myDepth;

    QVector<QPointF> mySeries;

    switch (graphType)
    {
        case outputType::soilTemperature :
            for (int z=0; z<myOut->nrLayers; z++)
            {
                myDepth = myOut->layerThickness * ((float)z + 0.5);

                myColor = myColorScale.getColor(z);
                myQColor = QColor(myColor->red, myColor->green, myColor->blue);
                myPen.setColor(myQColor);

                myTitle = "T-";
                myTitle.append(QString::number(myDepth,'f',3));

                mySeries = getProfileSeries(myOut, graphType, z);
                addCurve(myTitle, QwtPlotCurve::Lines, myPen, mySeries);
            }
            break;

        case outputType::soilWater :
            for (int z=0; z<myOut->nrLayers; z++)
            {
                myDepth = myOut->layerThickness * ((float)z + 0.5);

                myColor = myColorScale.getColor(z);
                myQColor = QColor(myColor->red, myColor->green, myColor->blue);
                myPen.setColor(myQColor);

                myTitle = "WC-";
                myTitle.append(QString::number(myDepth,'f',3));

                mySeries = getProfileSeries(myOut, graphType, z);
                addCurve(myTitle, QwtPlotCurve::Lines, myPen, mySeries);
            }
            break;

    }

    replot();
}








