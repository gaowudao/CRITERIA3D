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
    MyPicker( QwtPlot *plot );

    MyPicker( int xAxis = QwtPlot::xBottom,
              int yAxis = QwtPlot::yLeft,
              RubberBand rubberBand = CrossRubberBand,
              DisplayMode trackerMode = QwtPicker::AlwaysOn,
              QwtPlot *plot = nullptr );

    void selectPoint( const QPointF & point );
    void highlightCurve( bool isHightlight );

public slots:
    void slotSelected( const QPointF &pos);

private:
    QwtPlot *qwtPlot;
    QwtPlotCurve *selectedCurve;
    int selectedPointIndex;
};

#endif // MYPICKER_H
