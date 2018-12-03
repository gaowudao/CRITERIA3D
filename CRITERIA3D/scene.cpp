#include "scene.h"
#include "gis.h"

#include <QVector3D>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <QAttribute>
#include "crit3dProject.h"


Qt3DCore::QEntity *createScene(Crit3DProject* myProject, float magnify)
{
    int nrVertex = int(myProject->indexMap.maximum) + 1;
    QByteArray vertexPosition, vertexColor;
    vertexPosition.resize(nrVertex * 3 * int(sizeof(float)));
    vertexColor.resize(nrVertex * 3 * int(sizeof(float)));

    float *rawPosition = reinterpret_cast<float *>(vertexPosition.data());
    float *rawColor = reinterpret_cast<float *>(vertexColor.data());

    // Vertices
    long index;
    float x, y, z;
    Crit3DColor *myColor;
    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            index = long(myProject->indexMap.value[row][col]);
            if (index != long(myProject->indexMap.header->flag))
            {
                z = myProject->DTM.value[row][col];
                if (z != myProject->DTM.header->flag)
                {
                    gis::getUtmXYFromRowColSinglePrecision(*(myProject->DTM.header), row, col, &x, &y);
                    myColor = myProject->DTM.colorScale->getColor(z);

                    rawPosition[index*3] = x;
                    rawPosition[index*3+1] = y;
                    rawPosition[index*3+2] = z  *magnify;

                    /*
                    k3D.SlopeAmplification = 90 / GIS.slopeMap.ValMax

                    Aspect = GIS.aspectMap.Value(myRow, myCol)
                    Slope = GIS.slopeMap.Value(myRow, myCol)
                    If Aspect <> GIS.aspectMap.header.flag And Slope <> GIS.slopeMap.header.flag Then
                        Shadow = GetCosTable_NESW(Aspect) * max(3, Slope * SlopeAmplification)
                        color.red = valueRange(color.red + Shadow, 0, 255)
                        color.green = valueRange(color.green + Shadow, 0, 255)
                        color.blue = valueRange(color.blue + Shadow, 0, 255)
                    End If
                    */

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
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::VertexBaseType::Float);
    colorAttribute->setBuffer(vertexColorBuffer);
    colorAttribute->setVertexSize(3);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    // Indices
    QByteArray indexBufferData;
    indexBufferData.resize(nrVertex * 2 * 3 * int(sizeof(uint)));
    uint *indexData = reinterpret_cast<uint *>(indexBufferData.data());

    long v0, v1, v2, v3;
    index = 0;
    for (int row = 0; row < myProject->indexMap.header->nrRows; row++)
    {
        for (int col = 0; col < myProject->indexMap.header->nrCols; col++)
        {
            v0 = long(myProject->indexMap.value[row][col]);
            v1 = long(myProject->indexMap.header->flag);
            v2 = long(myProject->indexMap.header->flag);
            v3 = long(myProject->indexMap.header->flag);
            if (v0 != long(myProject->indexMap.header->flag))
            {
                if (row < (myProject->indexMap.header->nrRows-1))
                    v1 = long(myProject->indexMap.value[row+1][col]);
                if (row < (myProject->indexMap.header->nrRows-1) && col < (myProject->indexMap.header->nrCols-1))
                    v2 = long(myProject->indexMap.value[row+1][col+1]);
                if (col < (myProject->indexMap.header->nrCols-1))
                    v3 = long(myProject->indexMap.value[row][col+1]);

                if (v1 != long(myProject->indexMap.header->flag) && v2 != long(myProject->indexMap.header->flag))
                {
                    indexData[index*3] = uint(v0);
                    indexData[index*3+1] = uint(v1);
                    indexData[index*3+2] = uint(v2);
                    index++;
                }
                if (v2 != long(myProject->indexMap.header->flag) && v3 != long(myProject->indexMap.header->flag))
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
    indexBufferData.resize(nrTriangles * 3 * int(sizeof(uint)));
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
