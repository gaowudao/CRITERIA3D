#ifndef VIEWER3D_H
#define VIEWER3D_H

#include <QDialog>
#include <Qt3DExtras>
#include "crit3dProject.h"

class Viewer3D : public QDialog
{
    Q_OBJECT

    public:
        Viewer3D(QWidget *parent = 0);
        void initialize(Crit3DProject *project);

    protected:
        bool eventFilter(QObject *obj, QEvent *ev);
        void mouseMoveEvent(QMouseEvent *ev);
        void mousePressEvent(QMouseEvent *ev);
        void mouseReleaseEvent(QMouseEvent *ev);
        void wheelEvent(QWheelEvent *we);

    private:
        Crit3DProject *m_project;
        QPointer<Qt3DCore::QEntity> m_rootEntity;
        Qt3DExtras::Qt3DWindow *m_view;
        QPoint m_moveStartPoint;
        QMatrix4x4 m_cameraMatrix;
        float m_magnify;
};


#endif // VIEWER3D_H
