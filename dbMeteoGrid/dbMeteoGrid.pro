#-------------------------------------------------
#
# CRITERIA3D distribution
# dbMeteoGrid library
#
#-------------------------------------------------

QT       += network sql xml

QT       -= gui

TARGET = dbMeteoGrid

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

#CONFIG(debug, debug|release) {
#    TARGET = debug/dbMeteoGrid
#} else {
#    TARGET = release/dbMeteoGrid
#}

DEFINES += DBMETEOGRID_LIBRARY

INCLUDEPATH += ../mathFunctions ../gis ../crit3dDate  ../meteo ../utilities


SOURCES += \
        dbMeteoGrid.cpp \

HEADERS += \
        dbMeteoGrid.h

