#include "rubberBand.h"

RubberBand::RubberBand(QRubberBand::Shape s, QWidget *p) :
    QRubberBand(s,p)
{

}

void RubberBand::setOrigin(QPoint origin)
{
    this->origin = origin;
}

QPoint RubberBand::getOrigin()
{
    return this->origin;
}

void RubberBand::setFirstCorner(QPointF firstCorner)
{
    this->firstCorner = firstCorner;
}

QPointF RubberBand::getFirstCorner()
{
    return this->firstCorner;
}


void RubberBand::paintEvent(QPaintEvent *event)
{

    QRubberBand::paintEvent(event);

    QPainter p(this);
    p.setPen(QPen(Qt::black,2));
}

void RubberBand::resizeEvent(QResizeEvent *)
{
  this->resize(size());
}
