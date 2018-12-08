#include "viewer3d.h"
#include "scene.h"
#include "QHBoxLayout"


Viewer3D::Viewer3D(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_moveStartPoint.setX(-1);

    m_view = new Qt3DExtras::Qt3DWindow();

    m_view->installEventFilter(this);

    m_view->defaultFrameGraph()->setClearColor(QColor::fromRgbF(1, 1, 1, 1.0));
    setWindowTitle(QStringLiteral("3D"));
    m_view->setWidth(1000);
    m_view->setHeight(600);

    QWidget *container = QWidget::createWindowContainer(m_view);
    QSize screenSize = m_view->screen()->size();
    container->setMinimumSize(QSize(100, 100));
    container->setMaximumSize(screenSize);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    //QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);

    // Window geometry
    resize(int(parent->geometry().width() * 0.8), int(parent->geometry().height() * 0.8));
    move(parent->geometry().center() - QPoint(width() / 2, height() / 2));
}


void Viewer3D::initialize(Crit3DProject *project)
{
    m_project = project;
    gis::Crit3DUtmPoint utmCenter;
    float dz = maxValue(m_project->DTM.maximum - m_project->DTM.minimum, 10.f);
    float z = m_project->DTM.minimum + dz * 0.5f;
    float dy = m_project->DTM.header->nrRows * m_project->DTM.header->cellSize;
    float dx = m_project->DTM.header->nrCols * m_project->DTM.header->cellSize;
    utmCenter.x = m_project->DTM.header->llCorner->x + dx * 0.5f;
    utmCenter.y = m_project->DTM.header->llCorner->y + dy * 0.5f;
    float size = sqrt(dx*dy);
    float ratio = size / dz;
    m_magnify = maxValue(1., minValue(8.f, int(ratio / 5.f)));

    // Camera
    Qt3DRender::QCamera *camera = m_view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.f/9.f, 0.1f, 1000000);
    camera->setUpVector(QVector3D(0, 0, 1));
    camera->setPosition(QVector3D(utmCenter.x, utmCenter.y - dy*0.75, (z + dz*3) * m_magnify));
    camera->setViewCenter(QVector3D(utmCenter.x, utmCenter.y, z * m_magnify));

    // Set root object of the scene
    m_rootEntity = createScene(m_project, m_magnify);
    m_view->setRootEntity(m_rootEntity);
    //m_view->show();
}


Viewer3D::~Viewer3D()
{
    //delete ui;
}


bool Viewer3D::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() == QEvent::Wheel)
    {
        wheelEvent(dynamic_cast<QWheelEvent*>(ev));
        return true;
    }
    else if (ev->type() == QEvent::MouseButtonPress)
    {
        mousePressEvent(dynamic_cast<QMouseEvent*>(ev));
        return true;
    }
    else if (ev->type() == QEvent::MouseMove)
    {
        mouseMoveEvent(dynamic_cast<QMouseEvent*>(ev));
        return true;
    }
    else if (ev->type() == QEvent::MouseButtonRelease)
    {
        mouseReleaseEvent(dynamic_cast<QMouseEvent*>(ev));
        return true;
    }

    return QObject::eventFilter(obj, ev);
}


void Viewer3D::wheelEvent(QWheelEvent *we)
{
    Qt3DCore::QTransform* transform = m_view->camera()->transform();

    float scale = transform->scale();
    QPoint delta = we->angleDelta();
    float zoom_distance = scale * static_cast<float>(delta.y()) / 500.f;
    scale -= zoom_distance;
    scale = std::min(10.0000f, scale);
    scale = std::max(0.001f, scale);
    transform->setScale(scale);
}


void Viewer3D::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        m_moveStartPoint = ev->pos();
        m_cameraMatrix = m_view->camera()->transform()->matrix();
    }
}


void Viewer3D::mouseMoveEvent(QMouseEvent *ev)
{
    if (m_moveStartPoint.x() > -1)
    {
        QPoint delta = ev->pos() - m_moveStartPoint;
        float angle = static_cast<float>(QPoint::dotProduct(delta, delta)) / 100.f;
        QVector3D axis = QVector3D(delta.y(), delta.x(), 0);

        QMatrix4x4 rotationMatrix = Qt3DCore::QTransform::rotateAround(-m_view->camera()->position(), angle, axis);

        QMatrix4x4 matrix = rotationMatrix * m_cameraMatrix;
        m_view->camera()->transform()->setMatrix(matrix);
    }
}


void Viewer3D::mouseReleaseEvent(QMouseEvent *ev)
{
    if (m_moveStartPoint.x() > -1)
    {
        m_moveStartPoint.setX(-1);
        m_cameraMatrix = m_view->camera()->transform()->matrix();
    }
}

