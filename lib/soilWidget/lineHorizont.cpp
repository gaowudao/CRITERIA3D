#include "lineHorizont.h"
#include <qdebug.h>

LineHorizont::LineHorizont(QWidget* parent)
: QWidget(parent)
{

}

void LineHorizont::initialize(int x1, int y1, int width, int height, QString colorName)
{
    this->x1 = x1;
    this->y1 = y1;
    this->width = width;
    this->height = height;
    this->colorName = colorName;
}

void LineHorizont::draw()
{
    qDebug() << "draw" ;
    this->show();
}

int LineHorizont::getX1() const
{
    return x1;
}

void LineHorizont::setX1(int value)
{
    x1 = value;
}

int LineHorizont::getY1() const
{
    return y1;
}

void LineHorizont::setY1(int value)
{
    y1 = value;
}



void LineHorizont::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if (colorName == "rosso")
    {
        p.setBrush(Qt::red);
    }
    if (colorName == "blu")
    {
        p.setBrush(Qt::blue);
    }
    qDebug() << "x1 = " << x1;
    qDebug() << "y1 = " << y1;
    qDebug() << "width = " << width;
    qDebug() << "height = " << height;
    p.drawRect(x1,y1,width,height);


}

int LineHorizont::getHeight() const
{
    return height;
}

void LineHorizont::setHeight(int value)
{
    height = value;
}

int LineHorizont::getWidth() const
{
    return width;
}

void LineHorizont::setWidth(int value)
{
    width = value;
}

