#ifndef RASTEROBJECT_H
#define RASTEROBJECT_H

#include "MapGraphics_global.h"
#include "MapGraphicsObject.h"


class RasterObject : public MapGraphicsObject
{
    Q_OBJECT
public:
    explicit RasterObject(int xpos, int ypos, int width, int height, MapGraphicsObject *parent = 0);
    virtual ~RasterObject();

    //pure-virtual from MapGraphicsObject
    QRectF boundingRect() const;

    //pure-virtual from MapGraphicsObject
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
signals:
    
public slots:

protected:
    //virtual from MapGraphicsObject
    virtual void keyReleaseEvent(QKeyEvent *event);

private:
    int _xpos;
    int _ypos;
    int _width;
    int _height;
    
};

#endif // RASTEROBJECT_H
