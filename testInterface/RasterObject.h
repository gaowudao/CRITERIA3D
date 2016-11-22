#ifndef RASTEROBJECT_H
#define RASTEROBJECT_H

#include "MapGraphics_global.h"
#include "MapGraphicsObject.h"
#include "MapGraphicsView.h"



class RasterObject : public MapGraphicsObject
{
    Q_OBJECT
public:
    explicit RasterObject(int width, int height, MapGraphicsView* view, MapGraphicsObject *parent = 0);
    virtual ~RasterObject();

    //pure-virtual from MapGraphicsObject
    QRectF boundingRect() const;

    //pure-virtual from MapGraphicsObject
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void moveCenter();

protected:
    //virtual from MapGraphicsObject
    virtual void keyReleaseEvent(QKeyEvent *event); 

private:

    int _width;
    int _height;
    MapGraphicsView* _view;

};

#endif // RASTEROBJECT_H
