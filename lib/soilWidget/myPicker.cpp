#include "myPicker.h"
#include "qwt_symbol.h"

MyPicker::MyPicker(QwtPlot *plot) :
    QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, CrossRubberBand, QwtPicker::AlwaysOn, plot->canvas()),
    selectedCurve( nullptr ),
    qwtPlot( plot ),
    selectedPointIndex( -1 )
{
    connect( this, SIGNAL( selected( const QPointF ) ), this, SLOT( slotSelected( const QPointF ) ) );
}

void MyPicker::slotSelected( const QPointF &pos)
{
    qDebug() << "slotSelected " << pos;
    QwtPlotCurve *curve = nullptr;
    double minDist = std::numeric_limits<double>::max();
    int index = -1;

    const QwtPlotItemList& itmList = qwtPlot->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = ( QwtPlotCurve* )( *it );

            double d;
            int idx = c->closestPoint( pos.toPoint(), &d );
            if ( d < minDist )
            {
                curve = c;
                index = idx;
                minDist = d;
            }
        }
    }

    highlightCurve( false );
    selectedCurve = nullptr;
    selectedPointIndex = -1;

    if ( curve && minDist < 5 ) // 5 pixels tolerance
    {
        selectedCurve = curve;
        selectedPointIndex = index;
        highlightCurve( true );
    }
}

void MyPicker::highlightCurve( bool isHightlight )
{
    qDebug() << "highlightCurve " << isHightlight;
    if ( selectedCurve == nullptr )
        return;

    qDebug() << "selectedCurve " << selectedCurve;
    if ( isHightlight )
    {
        selectedCurve->setPen(Qt::red, 3);
    }
    else
    {
        selectedCurve->setPen(Qt::black, 1);
    }
    qwtPlot->replot();
}
