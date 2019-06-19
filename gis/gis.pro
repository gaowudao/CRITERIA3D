#----------------------------------------------------
#
#   GIS library
#   This project is part of CRITERIA-3D distribution
#
#----------------------------------------------------

QT  -= core gui

TEMPLATE = lib

DEFINES += _CRT_SECURE_NO_WARNINGS

CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/gis
    } else {
        TARGET = release/gis
    }
}
win32:{
    TARGET = gis
}

INCLUDEPATH += ../mathFunctions

SOURCES += gis.cpp \
    gisIO.cpp \
    color.cpp \
    map.cpp

HEADERS += gis.h \
    color.h \
    gisIO.h \
    map.h

