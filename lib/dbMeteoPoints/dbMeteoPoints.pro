#-----------------------------------------------------
#
#   dbMeteoPoints library
#   This project is part of CRITERIA-3D distribution
#
#-----------------------------------------------------

QT       += network sql

QT       -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/dbMeteoPoints
    } else {
        TARGET = release/dbMeteoPoints
    }
}
win32:{
    TARGET = dbMeteoPoints
}

DEFINES += DBMETEOPOINTS_LIBRARY

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo ../interpolation ../utilities

SOURCES += \
    download.cpp \
    dbArkimet.cpp \
    dbMeteoPointsHandler.cpp \
    dbAggregationsHandler..cpp \
    variablesList.cpp

HEADERS += \
    download.h \
    dbArkimet.h \
    dbMeteoPointsHandler.h \
    dbAggregationsHandler.h \
    variablesList.h


