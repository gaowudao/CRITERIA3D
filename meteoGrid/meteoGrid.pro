#-------------------------------------------------
#
# Project created by QtCreator 2018-03-07T09:37:23
#
#-------------------------------------------------
QT       += network sql

QT       -= gui

TARGET = meteoGrid
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions ../meteo ../gis

DEFINES += METEOGRID_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    meteoGrid.cpp \
    gridStructure.cpp

HEADERS += \
    meteoGrid.h \
    gridStructure.h

