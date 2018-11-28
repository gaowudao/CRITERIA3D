#include "scene.h"
#include "gis.h"

#include <QVector3D>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <QAttribute>


Qt3DCore::QEntity *createScene(gis::Crit3DRasterGrid *dtm, gis::Crit3DRasterGrid *indexGrid, float magnify)
{
    int nrVertex = indexGrid->maximum+1;
    QByteArray vertexPosition, vertexColor;
    vertexPosition.resize(nrVertex * 3 * sizeof(float));
    vertexColor.resize(nrVertex * 3 * sizeof(float));

    float *rawPosition = reinterpret_cast<float *>(vertexPosition.data());
    float *rawColor = reinterpret_cast<float *>(vertexColor.data());

    // Vertices
    long index;
    float x, y, z;
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
                    gis::getUtmXYFromRowColSinglePrecision(*(dtm->header), row, col, &x, &y);
                    myColor = dtm->colorScale->getColor(z);

                    rawPosition[index*3] = x;
                    rawPosition[index*3+1] = y;
                    rawPosition[index*3+2] = z  *magnify;

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

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::VertexBaseType::Float);
    positionAttribute->setBuffer(vertexPositionBuffer);
    positionAttribute->setVertexSize(3);
    //positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::VertexBaseType::Float);
    colorAttribute->setBuffer(vertexColorBuffer);
    colorAttribute->setVertexSize(3);
    //colorAttribute->setByteStride(3 * sizeof(float));
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    // Indices
    QByteArray indexBufferData;
    indexBufferData.resize(nrVertex * 2 * 3 * sizeof(uint));
    uint *indexData = reinterpret_cast<uint *>(indexBufferData.data());

    long v0, v1, v2, v3;
    index = 0;
    for (int row = 0; row < indexGrid->header->nrRows; row++)
    {
        for (int col = 0; col < indexGrid->header->nrCols; col++)
        {
            v0 = indexGrid->value[row][col];
            v1 = indexGrid->header->flag;
            v2 = indexGrid->header->flag;
            v3 = indexGrid->header->flag;
            if (v0 != indexGrid->header->flag)
            {
                if (row < (indexGrid->header->nrRows-1))
                    v1 = indexGrid->value[row+1][col];
                if (row < (indexGrid->header->nrRows-1) && col < (indexGrid->header->nrCols-1))
                    v2 = indexGrid->value[row+1][col+1];
                if (col < (indexGrid->header->nrCols-1))
                    v3 = indexGrid->value[row][col+1];

                if (v1 != indexGrid->header->flag && v2 != indexGrid->header->flag)
                {
                    indexData[index*3] = uint(v0);
                    indexData[index*3+1] = uint(v1);
                    indexData[index*3+2] = uint(v2);
                    index++;
                }
                if (v2 != indexGrid->header->flag && v3 != indexGrid->header->flag)
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
    indexBufferData.resize(nrTriangles * 3 * sizeof(uint));
    Qt3DRender::QBuffer *indexBuffer = new Qt3DRender::QBuffer();
    indexBuffer->setData(indexBufferData);

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setBuffer(indexBuffer);

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
    geometryRenderer->setVertexCount(nrTriangles*3);

    // Material
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial();

    // Entity
    Qt3DCore::QEntity *scene = new Qt3DCore::QEntity();
    scene->addComponent(material);
    scene->addComponent(geometryRenderer);

    return scene;
}
