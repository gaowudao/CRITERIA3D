#-------------------------------------------------
#
# CRITERIA3D distribution
# dbMeteoGrid library
#
#-------------------------------------------------

QT       += network sql xml

QT       -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/dbMeteoGrid
    } else {
        TARGET = release/dbMeteoGrid
    }
}
win32:{
    TARGET = dbMeteoGrid
}

DEFINES += DBMETEOGRID_LIBRARY

INCLUDEPATH += ../mathFunctions ../gis ../crit3dDate  ../meteo ../utilities


SOURCES += \
        dbMeteoGrid.cpp \

HEADERS += \
        dbMeteoGrid.h

