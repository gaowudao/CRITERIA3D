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

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo

DEFINES += METEOGRID_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    meteoGrid.cpp

HEADERS += \
    meteoGrid.h

