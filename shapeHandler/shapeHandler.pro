#--------------------------------------------------
#
# Shapefile handler
# this library is part of CRITERIA-3D distribution
#
# The library includes:
# - shapelib of Frank Warmerdam
# http://shapelib.maptools.org/
#
# - shapeobject.cpp of Erik Svensson
# https://github.com/blueluna/shapes
#
#--------------------------------------------------

QT    -= gui core

TARGET = shapeHandler
TEMPLATE = lib
CONFIG += staticlib

DEFINES += SHAPEHANDLER_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH =  shapelib

SOURCES += \
    shapelib/dbfopen.c      \
    shapelib/safileio.c     \
    shapelib/sbnsearch.c    \
    shapelib/shpopen.c      \
    shapelib/shptree.c      \
    shapeObject.cpp         \
    shapeHandler.cpp


HEADERS += \
    shapelib/shapefil.h     \
    shapeHandler.h          \
    shapeObject.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}
