#ifndef MYPICKER_H
#define MYPICKER_H

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>

// custom QwtPlotPicker to select a curve

class MyPicker : public QwtPlotPicker
{
    Q_OBJECT
public:
    MyPicker(QwtPlot *plot, QList<QwtPlotCurve*> allCurveList);

    MyPicker( int xAxis = QwtPlot::xBottom,
              int yAxis = QwtPlot::yLeft,
              RubberBand rubberBand = CrossRubberBand,
              DisplayMode trackerMode = QwtPicker::AlwaysOn,
              QwtPlot *plot = nullptr );

    void highlightCurve( bool isHightlight );
    int closestPoint( QwtPlotCurve& curve, const QPoint &pos, double *dist );

public slots:
    void slotSelected( const QPointF &pos);

private:
    QwtPlot *qwtPlot;
    QwtPlotCurve *selectedCurve;
    int selectedPointIndex;
    QList<QwtPlotCurve*> allCurveList;
};

#endif // MYPICKER_H
