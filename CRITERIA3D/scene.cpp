#include "scene.h"
#include "gis.h"

#include <QVector3D>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>


Qt3DCore::QEntity *createScene(gis::Crit3DRasterGrid *dtm, gis::Crit3DRasterGrid *indexGrid)
{
    QByteArray vertexBufferData;
    vertexBufferData.resize(5 * (3 + 3) * sizeof(float));

    // Vertices
    QVector3D v0(-1.0f, -1.0f, 0.0f);
    QVector3D v1(1.0f, -1.0f, 0.0f);
    QVector3D v2(-1.0f, 1.0f, 0.0f);
    QVector3D v3(1.0f, 1.0f, 1.0f);
    QVector3D v4(3.0f, 1.0f, 0.0f);

    // Colors
    QVector3D red(1.0f, 0.0f, 0.0f);
    QVector3D green(0.0f, 1.0f, 0.0f);

    QVector<QVector3D> vertices = QVector<QVector3D>()
            << v0 << green
            << v1 << green
            << v2 << green
            << v3 << red
            << v4 << green;

    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;

    for (const QVector3D &v : vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }

    Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer();
    vertexDataBuffer->setData(vertexBufferData);

    // Indices
    QByteArray indexBufferData;
    indexBufferData.resize(3 * 3 * sizeof(ushort));
    ushort *indexData = reinterpret_cast<ushort *>(indexBufferData.data());

    // 1
    indexData[0] = 0;
    indexData[1] = 1;
    indexData[2] = 2;
    // 2
    indexData[3] = 1;
    indexData[4] = 3;
    indexData[5] = 2;
    // 3
    indexData[6] = 1;
    indexData[7] = 4;
    indexData[8] = 3;

    Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer();
    indexDataBuffer->setData(indexBufferData);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(3 * sizeof(float));
    colorAttribute->setByteStride(6 * sizeof(float));
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setDataSize(1);
    //indexAttribute->setByteOffset(0);
    //indexAttribute->setByteStride(0);

    // Geometry
    Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();
    geometry->addAttribute(positionAttribute);
    geometry->addAttribute(colorAttribute);
    geometry->addAttribute(indexAttribute);

    // Geometry renderer
    Qt3DRender::QGeometryRenderer *geometryRenderer = new Qt3DRender::QGeometryRenderer;
    geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    geometryRenderer->setGeometry(geometry);
    geometryRenderer->setInstanceCount(1);
    geometryRenderer->setFirstVertex(0);
    geometryRenderer->setFirstInstance(0);
    // 3 faces of 3 points
    geometryRenderer->setVertexCount(9);

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial();

    // Entity
    Qt3DCore::QEntity *scene = new Qt3DCore::QEntity();
    scene->addComponent(material);
    scene->addComponent(geometryRenderer);

    return scene;
}
