#include "viewer3d.h"
#include "QHBoxLayout"

#include <QVector3D>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <QAttribute>
#include <QLabel>


Viewer3D::Viewer3D(QWidget *parent)
{
    m_project = nullptr;
    m_geometry = nullptr;
    m_rootEntity = nullptr;
    m_magnify = NODATA;
    m_size = NODATA;
    m_ratio = NODATA;
    m_nrVertex = 0;
    this->buildLookupTables();

    this->setWindowTitle(QStringLiteral("3D"));

    this->setAttribute(Qt::WA_DeleteOnClose);
    m_moveStartPoint.setX(-1);

    m_view = new Qt3DExtras::Qt3DWindow();
    m_view->installEventFilter(this);
    m_view->defaultFrameGraph()->setClearColor(QColor::fromRgbF(1, 1, 1, 1.0));

    QWidget *container = QWidget::createWindowContainer(m_view);
    QSize screenSize = m_view->screen()->size();
    container->setMinimumSize(QSize(100, 100));
    container->setMaximumSize(screenSize);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    //QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    QLabel* testLabel = new QLabel("    TEST    ");
    hLayout->addWidget(testLabel);

    // Window geometry
    this->resize(int(parent->geometry().width() * 0.8), int(parent->geometry().height() * 0.8));
    this->move(parent->geometry().center() - QPoint(width() / 2, height() / 2));
}


void Viewer3D::initialize(Crit3DProject *project)
{
    m_project = project;
    float dz = maxValue(m_project->DTM.maximum - m_project->DTM.minimum, 10.f);
    float z = m_project->DTM.minimum + dz * 0.5f;
    double dy = m_project->DTM.header->nrRows * m_project->DTM.header->cellSize;
    double dx = m_project->DTM.header->nrCols * m_project->DTM.header->cellSize;
    m_center.x = m_project->DTM.header->llCorner->x + dx * 0.5;
    m_center.y = m_project->DTM.header->llCorner->y + dy * 0.5;
    m_size = float(sqrt(dx*dy));
    m_ratio = m_size / dz;
    m_magnify = maxValue(1.f, minValue(10.f, m_ratio / 5.f));

    // Camera
    Qt3DRender::QCamera *camera = m_view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.f/9.f, 0.01f, 1000000.f);
    camera->setUpVector(QVector3D(0, 1, -1));
    camera->setPosition(QVector3D(float(m_center.x), float(m_center.y), (z + dz*5) * m_magnify));
    camera->setViewCenter(QVector3D(float(m_center.x), float(m_center.y), z * m_magnify));
    m_cameraPosition = camera->position();

    // Set root object of the scene
    createScene();

    /*Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(m_rootEntity);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);*/

    m_view->setRootEntity(m_rootEntity);
}


Viewer3D::~Viewer3D()
{
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
    if ((ev->button() == Qt::LeftButton) || (ev->button() == Qt::RightButton))
    {
        m_button = ev->button();
        m_moveStartPoint = ev->pos();
        m_cameraMatrix = m_view->camera()->transform()->matrix();
        m_cameraPosition = m_view->camera()->transform()->translation();
    }
}


void Viewer3D::mouseMoveEvent(QMouseEvent *ev)
{
    if (m_moveStartPoint.x() > -1)
    {
        QPoint delta = ev->pos() - m_moveStartPoint;
        if (m_button == Qt::RightButton)
        {  
            /*float scale = m_view->camera()->transform()->scale();
            float zoom_distance = scale * static_cast<float>(delta.y()) / 500.f;
            scale -= zoom_distance;
            scale = std::min(10.0000f, scale);
            scale = std::max(0.001f, scale);
            m_view->camera()->transform()->setScale(scale);*/

            /*float zoom = delta.y() * (m_size/10000000.f);
            QVector3D axis = QVector3D(1, 1, 0);
            QMatrix4x4 zoomMatrix = Qt3DCore::QTransform::rotateAround(-m_view->camera()->position(), zoom, axis);
            QMatrix4x4 matrix = zoomMatrix * m_cameraMatrix;
            m_view->camera()->transform()->setMatrix(matrix);*/

            float dx = delta.x() * (m_size/1000000.f);
            m_view->camera()->setUpVector(QVector3D(dx, 1, -1));
        }
        else if (m_button == Qt::LeftButton)
        {
            QVector3D translation = QVector3D(m_cameraPosition.x() + delta.x() * (m_size/1000.f),
                                              m_cameraPosition.y() - delta.y() * (m_size/1000.f),
                                              m_cameraPosition.z());
            m_view->camera()->transform()->setTranslation(translation);
        }
    }
}


void Viewer3D::mouseReleaseEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev);
    if (m_moveStartPoint.x() > -1)
    {
        m_moveStartPoint.setX(-1);
        m_cameraMatrix = m_view->camera()->transform()->matrix();
        m_cameraPosition = m_view->camera()->transform()->translation();
    }
}


void Viewer3D::buildLookupTables()
{
    for (int angle = 0; angle < 3600; angle++)
    {
        m_cosTable[angle] = cos(float(angle) / 10.f * float(DEG_TO_RAD));
    }
}


float Viewer3D::getCosTable(float angle)
{
    if (angle < 0)
        angle += 360.f;
    return m_cosTable[int(angle * 10)];
}


float getValueInRange(float value, float minimum, float maximum)
{
    if (value < minimum)
        return minimum;
    else if (value > maximum)
        return maximum;
    else
        return value;
}


void setVertexColor(float* vertexColor, int vertexIndex, Crit3DColor* myColor, float shadow)
{
    float red = getValueInRange(myColor->red + shadow, 0, 255);
    float green = getValueInRange(myColor->green + shadow, 0, 255);
    float blue = getValueInRange(myColor->blue + shadow, 0, 255);

    vertexColor[vertexIndex*3] = red / 256.f;
    vertexColor[vertexIndex*3+1] = green / 256.f;
    vertexColor[vertexIndex*3+2] = blue / 256.f;
}


void Viewer3D::cleanScene()
{
    if (m_rootEntity != nullptr)
    {
        m_vertexPositionArray.clear();
        m_vertexColorArray.clear();
        m_triangleIndexArray.clear();
        m_geometry->clearPropertyTracking(Qt3DRender::QAttribute::defaultPositionAttributeName());
        m_geometry->clearPropertyTracking(Qt3DRender::QAttribute::defaultColorAttributeName());
        m_geometry->clearPropertyTracking(Qt3DRender::QAttribute::defaultJointIndicesAttributeName());
        m_rootEntity.clear();
    }
}


void Viewer3D::createScene()
{
    m_nrVertex = int(m_project->indexMap.maximum) + 1;

    m_vertexPositionArray.resize(m_nrVertex * 3 * int(sizeof(float)));
    m_vertexColorArray.resize(m_nrVertex * 3 * int(sizeof(float)));
    m_triangleIndexArray.resize(m_nrVertex * 2 * 3 * int(sizeof(uint)));

    float *vertexPosition = reinterpret_cast<float *>(m_vertexPositionArray.data());
    float *vertexColor = reinterpret_cast<float *>(m_vertexColorArray.data());
    uint *indexData = reinterpret_cast<uint *>(m_triangleIndexArray.data());

    float SlopeAmplification = 90.f / m_project->radiationMaps->slopeMap->maximum;
    float myAspect, mySlope, shadow;

    // Vertices
    long index;
    float x, y, z;
    Crit3DColor *myColor;
    for (int row = 0; row < m_project->indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < m_project->indexMap.header->nrCols; col++)
        {
            index = long(m_project->indexMap.value[row][col]);
            if (index != long(m_project->indexMap.header->flag))
            {
                z = m_project->DTM.value[row][col];
                if (z != m_project->DTM.header->flag)
                {
                    gis::getUtmXYFromRowColSinglePrecision(*(m_project->DTM.header), row, col, &x, &y);

                    vertexPosition[index*3] = x;
                    vertexPosition[index*3+1] = y;
                    vertexPosition[index*3+2] = z  * m_magnify;

                    myColor = m_project->DTM.colorScale->getColor(z);

                    shadow = 0.f;
                    myAspect = m_project->radiationMaps->aspectMap->value[row][col];
                    mySlope = m_project->radiationMaps->slopeMap->value[row][col];
                    if ((myAspect != m_project->radiationMaps->aspectMap->header->flag)
                            && (mySlope != m_project->radiationMaps->slopeMap->header->flag))
                    {
                            shadow = getCosTable(myAspect) * mySlope * SlopeAmplification;
                    }

                    setVertexColor(vertexColor, index, myColor, shadow);
                }
            }
        }
    }

    Qt3DRender::QBuffer *vertexPositionBuffer = new Qt3DRender::QBuffer();
    vertexPositionBuffer->setData(m_vertexPositionArray);
    Qt3DRender::QBuffer *vertexColorBuffer = new Qt3DRender::QBuffer();
    vertexColorBuffer->setData(m_vertexColorArray);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::VertexBaseType::Float);
    positionAttribute->setBuffer(vertexPositionBuffer);
    positionAttribute->setVertexSize(3);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::VertexBaseType::Float);
    colorAttribute->setBuffer(vertexColorBuffer);
    colorAttribute->setVertexSize(3);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    // Indices
    long v0, v1, v2, v3;
    index = 0;
    for (int row = 0; row < m_project->indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < m_project->indexMap.header->nrCols; col++)
        {
            v0 = long(m_project->indexMap.value[row][col]);
            v1 = long(m_project->indexMap.header->flag);
            v2 = long(m_project->indexMap.header->flag);
            v3 = long(m_project->indexMap.header->flag);
            if (v0 != long(m_project->indexMap.header->flag))
            {
                if (row < (m_project->indexMap.header->nrRows-1))
                    v1 = long(m_project->indexMap.value[row+1][col]);
                if (row < (m_project->indexMap.header->nrRows-1) && col < (m_project->indexMap.header->nrCols-1))
                    v2 = long(m_project->indexMap.value[row+1][col+1]);
                if (col < (m_project->indexMap.header->nrCols-1))
                    v3 = long(m_project->indexMap.value[row][col+1]);

                if (v1 != long(m_project->indexMap.header->flag) && v2 != long(m_project->indexMap.header->flag))
                {
                    indexData[index*3] = uint(v0);
                    indexData[index*3+1] = uint(v1);
                    indexData[index*3+2] = uint(v2);
                    index++;
                }
                if (v2 != long(m_project->indexMap.header->flag) && v3 != long(m_project->indexMap.header->flag))
                {
                    indexData[index*3] = uint(v2);
                    indexData[index*3+1] = uint(v3);
                    indexData[index*3+2] = uint(v0);
                    index++;
                }
            }
        }
    }

    long nrTriangles = index;
    Qt3DRender::QBuffer *indexBuffer = new Qt3DRender::QBuffer();
    indexBuffer->setData(m_triangleIndexArray);

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setBuffer(indexBuffer);

    // Geometry
    m_geometry = new Qt3DRender::QGeometry();
    m_geometry->addAttribute(positionAttribute);
    m_geometry->addAttribute(colorAttribute);
    m_geometry->addAttribute(indexAttribute);

    // Geometry renderer
    Qt3DRender::QGeometryRenderer *geometryRenderer = new Qt3DRender::QGeometryRenderer;
    geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    geometryRenderer->setGeometry(m_geometry);
    geometryRenderer->setInstanceCount(1);
    geometryRenderer->setFirstVertex(0);
    geometryRenderer->setFirstInstance(0);
    geometryRenderer->setVertexCount(nrTriangles*3);

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial();

    m_rootEntity = new Qt3DCore::QEntity();

    // Entity
    m_rootEntity->addComponent(material);
    m_rootEntity->addComponent(geometryRenderer);
}

