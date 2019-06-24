#-------------------------------------------------
#
# CRITERIA3D distribution
# dbMeteoPoints library
#
#-------------------------------------------------

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

INCLUDEPATH += ../mathFunctions ../gis ../utilities ../meteo ../crit3dDate ../interpolation

SOURCES += \
    download.cpp \
    variableslist.cpp \
    dbArkimet.cpp \
    dbMeteoPointsHandler.cpp

HEADERS += \
    download.h \
    variableslist.h \
    dbArkimet.h \
    dbMeteoPointsHandler.h


