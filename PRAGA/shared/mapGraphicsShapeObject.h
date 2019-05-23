#ifndef MAPGRAPHICSSHAPEOBJECT_H
#define MAPGRAPHICSSHAPEOBJECT_H

    #include "MapGraphics_global.h"
    #include "MapGraphicsObject.h"
    #include "MapGraphicsView.h"

    #ifndef SHAPEHANDLER_H
        #include "shapeHandler.h"
    #endif
    #ifndef MAP_H
        #include "map.h"
    #endif

    class MapGraphicsShapeObject : public MapGraphicsObject
    {
        Q_OBJECT
    private:
        Crit3DShapeHandler* shapePtr;
        MapGraphicsView* view;
        gis::Crit3DGeoMap* geoMap;
        bool isDrawing;

    public:
        /*!
         * \brief mapShapeObject constructor
         * \param view a MapGraphicsView pointer
         * \param parent MapGraphicsObject
         */
        explicit MapGraphicsShapeObject(MapGraphicsView* view, MapGraphicsObject *parent = nullptr);

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
        //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

        void setShape(Crit3DShapeHandler* shapePtr);
        void updateCenter();
        void setDrawing(bool value);
    };



#endif // MAPGRAPHICSSHAPEOBJECT_H
