#-------------------------------------------------
#
# Shapefile handler
# this library is part of CRITERIA3D distribution
#
# it uses shapelib of Frank Warmerdam:
# http://shapelib.maptools.org/
#
# and portion of code of shapes of Erik Svensson:
# https://github.com/blueluna/shapes
#
#-------------------------------------------------

QT    -= gui core

TARGET = shapeHandler
TEMPLATE = lib
CONFIG += staticlib

DEFINES += SHAPEHANDLER_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH =  shapelib

SOURCES += \
    shape.cpp \
    shapeHandler.cpp \
    shapelib/dbfopen.c \
    shapelib/safileio.c \
    shapelib/sbnsearch.c \
    shapelib/shpopen.c \
    shapelib/shptree.c


HEADERS += \
    shape.h \
    shapeHandler.h \
    shapelib/shapefil.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}
