#ifndef RUBBERBAND_H
#define RUBBERBAND_H

#include <QObject>
#include <QRubberBand>
#include <QPainter>
#include <QPaintEvent>

class RubberBand : public QRubberBand
{
    Q_OBJECT
public:
    RubberBand(Shape s, QWidget *p = Q_NULLPTR);
    void setOrigin(QPoint origin);
    QPoint getOrigin();
    void setFirstCorner(QPointF firstCorner);
    QPointF getFirstCorner();
private:
    QPoint origin;
    QPointF firstCorner;

signals:

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *);

public slots:

};

#endif // RUBBERBAND_H
