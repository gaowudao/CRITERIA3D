#include "myPicker.h"
#include "qwt_symbol.h"

MyPicker::MyPicker(QwtPlot *plot, QList<QwtPlotCurve *> allCurveList) :
    QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, CrossRubberBand, QwtPicker::AlwaysOn, plot->canvas()),
    qwtPlot(plot),
    allCurveList(allCurveList)
{
    connect( this, SIGNAL( selected( const QPointF ) ), this, SLOT( slotSelected( const QPointF ) ) );
}

void MyPicker::slotSelected( const QPointF &pos)
{

    QwtPlotCurve *curveFound = nullptr;
    double minDist = std::numeric_limits<double>::max();
    int curveIndex = -1;

    for ( int i = 0; i < allCurveList.size(); i++ )
    {
        QwtPlotCurve *c = allCurveList[i];
        double d = 0;
        if (closestPoint( *c, pos, &d ) != -1)
        {
            if ( d < minDist )
            {
                curveFound = c;
                curveIndex = i;
                minDist = d;
            }
        }

    }

    if ( curveFound && minDist < pos.x()/10) // tolerance (f(x) x is logarithmic)
    {
        selectedCurveIndex = curveIndex;
        highlightCurve( true );
    }
    else
    {
        highlightCurve(false);  // clear previous selection
        selectedCurveIndex = -1;
    }
}

int MyPicker::getSelectedCurveIndex() const
{
    return selectedCurveIndex;
}

void MyPicker::setSelectedCurveIndex(int value)
{
    selectedCurveIndex = value;
}

void MyPicker::highlightCurve( bool isHightlight )
{

    for ( int i = 0; i < allCurveList.size(); i++ )
    {
        if ( isHightlight && i == selectedCurveIndex)
        {
            allCurveList[i]->setPen(Qt::red, 3);
        }
        else
        {
            allCurveList[i]->setPen(Qt::black, 1);
        }
    }
    qwtPlot->replot();
}

int MyPicker::closestPoint( QwtPlotCurve& curve, const QPointF &pos, double *dist )
{

    const size_t numSamples = curve.dataSize();

    if ( curve.plot() == nullptr || numSamples <= 0 )
        return -1;

    const QwtSeriesData<QPointF> *series = curve.data();
    const double xPos = pos.x();
    const double yPos = pos.y();

    int index = -1;
    double dmin = std::numeric_limits<double>::max();

    for ( uint i = 0; i < numSamples; i++ )
    {
        const QPointF sample = series->sample( i );
        const double cx = abs(sample.x()  - xPos);
        const double cy = abs(sample.y()  - yPos);

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
