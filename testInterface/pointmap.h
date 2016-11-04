#ifndef POINTMAP_H
#define POINTMAP_H

#include "MapGraphicsView.h"

#include <QPoint>

class PointMap : public MapGraphicsView
{
    Q_OBJECT
public:
    explicit PointMap(QWidget * parent = 0);
    PointMap(MapGraphicsScene * scene, QWidget * parent = 0);

signals:
    void mapClicked(QPoint pos);

public slots:

protected slots:
    //virtual from MapGraphicsView
    virtual void handleChildMousePress(QMouseEvent *event);

    //virtual from MapGraphicsView
    virtual void handleChildMouseRelease(QMouseEvent *event);

private:
    bool _clickActive;
    QPoint _clickPos;

};

#endif // POINTMAP_H
