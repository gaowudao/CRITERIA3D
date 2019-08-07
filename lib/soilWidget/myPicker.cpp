#include "myPicker.h"
#include "qwt_symbol.h"

MyPicker::MyPicker(QwtPlot *plot, QList<QwtPlotCurve *> allCurveList) :
    QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, CrossRubberBand, QwtPicker::AlwaysOn, plot->canvas()),
    qwtPlot(plot),
    selectedCurve( nullptr ),
    selectedPointIndex( -1 ),
    allCurveList(allCurveList)
{
    connect( this, SIGNAL( selected( const QPointF ) ), this, SLOT( slotSelected( const QPointF ) ) );
}

void MyPicker::slotSelected( const QPointF &pos)
{
    qDebug() << "slotSelected " << pos;
    QwtPlotCurve *curveFound = nullptr;
    double minDist = std::numeric_limits<double>::max();
    int closestIndex = -1;

//    const QwtPlotItemList& itmList = qwtPlot->itemList();
//    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it )
//    {
//        qDebug() << "1" ;
//        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
//        {
//            qDebug() << "a" ;
//            QwtPlotCurve *c = ( QwtPlotCurve* )( *it );
//            double d;
//            //int idx = c->closestPoint( pos.toPoint(), &d );
//            qDebug() << "minDist " << minDist;
//            int idx = closestPoint( *c, pos.toPoint(), &d );
//            if ( d < minDist )
//            {
//                qDebug() << "d < minDist " << d;
//                qDebug() << "minDist " << minDist;
//                qDebug() << "c " << c;
//                curve = c;
//                index = idx;
//                minDist = d;
//            }
//        }
//    }

    for ( int i = 0; i < allCurveList.size(); i++ )
    {
        qDebug() << "curve number" << i;
        QwtPlotCurve *c = allCurveList[i];
        double d = 0;
        //int idx = c->closestPoint( pos.toPoint(), &d );
        qDebug() << "minDist " << minDist;
        int index = closestPoint( *c, pos.toPoint(), &d );
        if ( d < minDist )
        {
            qDebug() << "d < minDist " << d;
            qDebug() << "minDist " << minDist;
            qDebug() << "c " << c;
            curveFound = c;
            closestIndex = index;
            minDist = d;
        }
    }
qDebug() << "minDist " << minDist;
    if ( curveFound && minDist < 10 ) // 10 pixels tolerance
    {
        selectedCurve = curveFound;
        selectedPointIndex = closestIndex;
        highlightCurve( true );
    }
    else
    {
        highlightCurve(false);  // clear previous selection
        selectedCurve = nullptr;
        selectedPointIndex = -1;
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

int MyPicker::closestPoint( QwtPlotCurve& curve, const QPoint &pos, double *dist )
{
    const size_t numSamples = curve.dataSize();

    if ( curve.plot() == nullptr || numSamples <= 0 )
        return -1;

    const QwtSeriesData<QPointF> *series = curve.data();

    const QwtScaleMap xMap = curve.plot()->canvasMap( curve.xAxis() );
    const QwtScaleMap yMap = curve.plot()->canvasMap( curve.yAxis() );

    const double xPos = xMap.transform( pos.x() );
    const double yPos = yMap.transform( pos.y() );

    int index = -1;
    double dmin = std::numeric_limits<double>::max();

    for ( uint i = 0; i < numSamples; i++ )
    {
        const QPointF sample = series->sample( i );

        const double cx = xMap.transform( sample.x() ) - xPos;
        const double cy = yMap.transform( sample.y() ) - yPos;

        const double dist = sqrt( pow(cx,2) + pow(cy,2) );
        if ( dist < dmin )
        {
            index = i;
            dmin = dist;
        }
    }
    if ( dist )
        *dist = dmin;

    return index;
}
