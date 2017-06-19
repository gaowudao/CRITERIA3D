#ifndef RASTEROBJECT_H
#define RASTEROBJECT_H

#include "MapGraphics_global.h"
#include "MapGraphicsObject.h"
#include "MapGraphicsView.h"
#include "colorlegend.h"
#include "map.h"
#include "project.h"


/*!
 * \brief The RasterObject class
 */
class RasterObject : public MapGraphicsObject
{
    Q_OBJECT
public:
    /*!
     * \brief RasterObject constructor
     * \param view a MapGraphicsView pointer
     * \param parent MapGraphicsObject
     */
    explicit RasterObject(MapGraphicsView* view, MapGraphicsObject *parent = 0);

    /*!
     * \brief boundingRect pure-virtual from MapGraphicsObject
     * Defines the outer bounds of the item as a rectangle; all painting must be restricted to inside an item's bounding rect.
     * \return the bounding rect QRectF
     */
    QRectF boundingRect() const;

    /*!
     * \brief paint pure-virtual from MapGraphicsObject
     * \param painter a QPainter pointer
     * \param option a QStyleOptionGraphicsItem pointer
     * \param widget a QWidget pointer
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void updateCenter();
    void setDrawing(bool value);
    void setColorLegend(ColorLegend* myLegend);

protected:
    //virtual from MapGraphicsObject

private:
    MapGraphicsView* _view;
    gis::Crit3DGeoMap* geoMap;
    ColorLegend* legend;
    bool isDrawing;

    bool setMapResolution();
    bool drawRaster(Project* myProject, gis::Crit3DRasterGrid *myRaster, QPainter* myPainter);
};


#endif // RASTEROBJECT_H
