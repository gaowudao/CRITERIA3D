/*******************************************************************
 * This code is based on shapeobject.cpp of Erik Svensson
 * https://github.com/blueluna/shapes
 *
 * Copyright (c) 2012 Erik Svensson
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:

 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 *******************************************************************/

#include <string.h>
#include "shapeObject.h"


int ShapeObject::getPartCount() const
{
    return partCount;
}

ShapeObject::ShapeObject()
    : index(-1), type(0), vertexCount(0), vertices(nullptr) {}

ShapeObject::ShapeObject(const SHPObject* obj)
    : index(-1), type(0), vertexCount(0), vertices(nullptr)
{
    assign(obj);
}

ShapeObject::ShapeObject(const ShapeObject &other)
    : index(-1), type(0), vertexCount(0), vertices(nullptr)
{
    assign(other);
}

ShapeObject::~ShapeObject()
{
    destroy();
}

ShapeObject& ShapeObject::operator = (const ShapeObject& other)
{
    assign(other);
    return *this;
}

void ShapeObject::destroy()
{
    if (vertexCount > 0) {
        delete [] vertices;
        vertices = nullptr;
        vertexCount = 0;
        partCount = 0;
    }
    type = 0;
    index = -1;
}

void ShapeObject::assign(const SHPObject* obj)
{
    if (obj != nullptr) {
        if (index >= 0) {
            destroy();
        }
        index = obj->nShapeId;
        type = obj->nSHPType;
        vertexCount = unsigned(obj->nVertices);
        if (vertexCount > 0) {
            vertices = new Point<double> [vertexCount];

            double *xptr = obj->padfX, *yptr = obj->padfY;
            Point<double> *pptr = vertices;
            Point<double> *pend = pptr + vertexCount;
            while (pptr < pend) {
                pptr->set(*xptr, *yptr);
                xptr++;
                yptr++;
                pptr++;
            }
        }
        bounds.ymin = obj->dfYMin;
        bounds.xmin = obj->dfXMin;
        bounds.ymax = obj->dfYMax;
        bounds.xmax = obj->dfXMax;

        int *ps = obj->panPartStart;
        int *pt = obj->panPartType;
        Part part;
        parts.clear();
        partCount = obj->nParts;
        for (int n = 0; n < obj->nParts; n++) {
            part.type = *pt;
            part.offset = *ps;
            if ((n+1) == obj->nParts) {
                part.length = int(vertexCount) - (*ps);
            }
            else {
                part.length = *(ps+1) - *ps;
            }
            parts.push_back(part);
            ps++;
            pt++;
        }
    }
}

void ShapeObject::assign(const ShapeObject& other)
{
    if (&other != this) {
        if (index >= 0) {
            destroy();
        }
        index = other.index;
        type = other.type;
        vertexCount = other.vertexCount;
        partCount = other.partCount;
        if (vertexCount > 0) {
            vertices = new Point<double> [vertexCount];
            memcpy(vertices, other.vertices, other.vertexCount * sizeof(Point<double>));
        }
        bounds = other.bounds;
        parts = other.parts;
    }
}

bool ShapeObject::isValid() const
{
    return index >= 0;
}

int ShapeObject::getIndex() const
{
    return index;
}

int ShapeObject::getType() const
{
    return type;
}

std::string ShapeObject::getTypeString() const
{
    return getShapeTypeAsString(type);
}

unsigned int ShapeObject::getVertexCount() const
{
    return vertexCount;
}

const Point<double>* ShapeObject::getVertices() const
{
    return const_cast<const Point<double>*>(vertices);
}

Box<double> ShapeObject::getBounds() const
{
    return bounds;
}

std::vector<ShapeObject::Part> ShapeObject::getParts() const
{
    return parts;
}

double ShapeObject::polygonArea(int indexPart)
{
    double area = 0;
    if (indexPart > getPartCount())
    {
        return -9999;
    }
    long offSet = getParts().at(indexPart).offset;
    long length = getParts().at(indexPart).length;

    for (long i = offSet; i < length; i++)
    {
            int j = (i + 1) % length;
            area += vertices[i].x * vertices[j].y;
            area -= vertices[j].x * vertices[i].y;
    }
    return area / 2;

}

bool ShapeObject::isClockWise(int indexPart)
{
    return polygonArea(indexPart) < 0;
}

std::string getShapeTypeAsString(int shapeType)
{
    std::string shape;
    switch (shapeType) {
    case SHPT_NULL:
        shape = "None";
        break;
    case SHPT_POINT:
        shape = "2D Point";
        break;
    case SHPT_ARC:
        shape = "2D Arc";
        break;
    case SHPT_POLYGON:
        shape = "2D Polygon";
        break;
    case SHPT_MULTIPOINT:
        shape = "2D Multi-point";
        break;
    case SHPT_POINTZ:
        shape = "3D Point";
        break;
    case SHPT_ARCZ:
        shape = "3D Arc";
        break;
    case SHPT_POLYGONZ:
        shape = "3D Polygon";
        break;
    case SHPT_MULTIPOINTZ:
        shape = "3D Multi-point";
        break;
    case SHPT_POINTM:
        shape = "2D Measure Point";
        break;
    case SHPT_ARCM:
        shape = "2D Measure Arc";
        break;
    case SHPT_POLYGONM:
        shape = "2D Measure Polygon";
        break;
    case SHPT_MULTIPOINTM:
        shape = "2D Measure Multi-point";
        break;
    case SHPT_MULTIPATCH:
        shape = "Multi-patch";
        break;
    default:
        shape = "Unknown";
    }
    return shape;
}
