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


static float cosTable[3600];


void buildLookupTables()
{
    for (int angle = 0; angle < 3600; angle++)
    {
        cosTable[angle] = cos(float(angle) / 10.f * float(DEG_TO_RAD));
    }
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


float getCosTable(float angle)
{
    if (angle < 0)
        angle += 360.f;
    return cosTable[int(angle * 10)];
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


Qt3DCore::QEntity *createScene(Crit3DProject* myProject, float magnify)
{
    QByteArray vertexPositionArray, vertexColorArray, triangleIndexArray;

    //vertexPositionArray.clear();
    //vertexColorArray.clear();
    //triangleIndexArray.clear();

    int nrVertex = int(myProject->indexMap.maximum) + 1;

    vertexPositionArray.resize(nrVertex * 3 * int(sizeof(float)));
    vertexColorArray.resize(nrVertex * 3 * int(sizeof(float)));
    triangleIndexArray.resize(nrVertex * 2 * 3 * int(sizeof(uint)));

    float *vertexPosition = reinterpret_cast<float *>(vertexPositionArray.data());
    float *vertexColor = reinterpret_cast<float *>(vertexColorArray.data());
    uint *indexData = reinterpret_cast<uint *>(triangleIndexArray.data());

    float SlopeAmplification = 90.f / myProject->radiationMaps->slopeMap->maximum;
    float myAspect, mySlope, shadow;

    buildLookupTables();

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

                    vertexPosition[index*3] = x;
                    vertexPosition[index*3+1] = y;
                    vertexPosition[index*3+2] = z  *magnify;

                    myColor = myProject->DTM.colorScale->getColor(z);

                    shadow = 0.f;
                    myAspect = myProject->radiationMaps->aspectMap->value[row][col];
                    mySlope = myProject->radiationMaps->slopeMap->value[row][col];
                    if ((myAspect != myProject->radiationMaps->aspectMap->header->flag)
                            && (mySlope != myProject->radiationMaps->slopeMap->header->flag))
                    {
                            shadow = getCosTable(myAspect) * mySlope * SlopeAmplification;
                    }

                    setVertexColor(vertexColor, index, myColor, shadow);
                }
            }
        }
    }

    Qt3DRender::QBuffer *vertexPositionBuffer = new Qt3DRender::QBuffer();
    vertexPositionBuffer->setData(vertexPositionArray);
    Qt3DRender::QBuffer *vertexColorBuffer = new Qt3DRender::QBuffer();
    vertexColorBuffer->setData(vertexColorArray);

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
    Qt3DRender::QBuffer *indexBuffer = new Qt3DRender::QBuffer();
    indexBuffer->setData(triangleIndexArray);

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

    Qt3DCore::QEntity *scene = new Qt3DCore::QEntity();

    // Entity
    scene->addComponent(material);
    scene->addComponent(geometryRenderer);

    return scene;
}
