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
    int nrVertex = indexGrid->maximum+1;
    QByteArray vertexPosition, vertexColor;
    vertexPosition.resize(nrVertex * 3 * sizeof(float));
    vertexColor.resize(nrVertex * 3 * sizeof(float));

    float *rawPosition = reinterpret_cast<float *>(vertexPosition.data());
    float *rawColor = reinterpret_cast<float *>(vertexColor.data());

    // Vertices
    long index;
    double x, y;
    float z;
    Crit3DColor *myColor;
    for (int row = 0; row < indexGrid->header->nrRows; row++)
    {
        for (int col = 0; col < indexGrid->header->nrCols; col++)
        {
            index = indexGrid->value[row][col];
            if (index != indexGrid->header->flag)
            {
                z = dtm->value[row][col];
                if (z != dtm->header->flag)
                {
                    gis::getUtmXYFromRowCol(*(dtm->header), row, col, &x, &y);
                    myColor = dtm->colorScale->getColor(z);

                    rawPosition[index*3] = float(x);
                    rawPosition[index*3+1] = float(y);
                    rawPosition[index*3+2] = z;

                    rawColor[index*3] = float(myColor->red) / 256.f;
                    rawColor[index*3+1] = float(myColor->green) / 256.f;
                    rawColor[index*3+2] = float(myColor->blue) / 256.f;
                }
            }
        }
    }

    Qt3DRender::QBuffer *vertexPositionBuffer = new Qt3DRender::QBuffer();
    vertexPositionBuffer->setData(vertexPosition);
    Qt3DRender::QBuffer *vertexColorBuffer = new Qt3DRender::QBuffer();
    vertexColorBuffer->setData(vertexColor);

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
    positionAttribute->setBuffer(vertexPositionBuffer);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexColorBuffer);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(0);
    colorAttribute->setByteStride(3 * sizeof(float));
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
