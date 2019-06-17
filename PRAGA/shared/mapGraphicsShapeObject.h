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

    struct latLonPoint
    {
        double lat;
        double lon;
    };

    class MapGraphicsShapeObject : public MapGraphicsObject
    {
        Q_OBJECT
    private:
        MapGraphicsView* view;
        Crit3DShapeHandler* shapePointer;
        gis::Crit3DGeoMap* geoMap;
        std::vector< std::vector<ShapeObject::Part> > shapeParts;
        std::vector< std::vector<bool> > hole;
        std::vector< gis::Crit3DGeoBox > geoBounds;
        std::vector< std::vector<latLonPoint> > geoPoints;
        bool isDrawing;

        void setMapResolution();
        void drawShape(QPainter* myPainter);
        QPointF getPoint(latLonPoint geoPoint);

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
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

        bool initializeUTM(Crit3DShapeHandler* shapePtr, const gis::Crit3DGisSettings& gisSettings);
        void setShape(Crit3DShapeHandler* shapePtr);
        Crit3DShapeHandler* getShapePointer();

        void setDrawing(bool value);
        void updateCenter();
        void clear();
    };



#endif // MAPGRAPHICSSHAPEOBJECT_H
