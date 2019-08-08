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
    int closestPoint(QwtPlotCurve& curve, const QPointF &pos, double *dist );

    int getSelectedCurveIndex() const;
    void setSelectedCurveIndex(int value);

public slots:
    void slotSelected( const QPointF &pos);

private:
    QwtPlot *qwtPlot;
    int selectedCurveIndex;
    QList<QwtPlotCurve*> allCurveList;
};

#endif // MYPICKER_H
