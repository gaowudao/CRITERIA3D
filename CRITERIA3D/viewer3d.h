#ifndef VIEWER3D_H
#define VIEWER3D_H

#include <Qt3DExtras>
#include "crit3dProject.h"

class Viewer3D : public QWidget
{
    Q_OBJECT

    public:
        Viewer3D(QWidget *parent = nullptr);
        void initialize(Crit3DProject *project);
        ~Viewer3D();

    protected:
        bool eventFilter(QObject *obj, QEvent *ev);
        void mouseMoveEvent(QMouseEvent *ev);
        void mousePressEvent(QMouseEvent *ev);
        void mouseReleaseEvent(QMouseEvent *ev);
        void wheelEvent(QWheelEvent *we);

    private:
        bool isCameraChanging;
        Qt3DExtras::Qt3DWindow *m_view;
        QPoint m_moveStartPoint;
        QMatrix4x4 m_cameraMatrix;
        QVector3D m_cameraPosition;
        float m_rotationZ;
        QByteArray m_vertexPositionArray;
        QByteArray m_vertexColorArray;
        QByteArray m_triangleIndexArray;
        Crit3DProject *m_project;
        Qt3DRender::QGeometry *m_geometry;
        QPointer<Qt3DCore::QEntity> m_rootEntity;

        float m_x_angle;
        float m_magnify;
        float m_zoomLevel;
        float m_size;
        float m_ratio;
        float m_cosTable[3600];
        int m_nrVertex;
        gis::Crit3DUtmPoint m_center;
        Qt::MouseButton m_button;

        void buildLookupTables();
        float getCosTable(float angle);
        void createScene();
        void cleanScene();
};


#endif // VIEWER3D_H
