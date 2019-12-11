#-----------------------------------------------------
#
#   project library
#   This project is part of CRITERIA-3D distribution
#
#-----------------------------------------------------

QT       += network sql

QT       -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release
QMAKE_CXXFLAGS += -std=c++11

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/project
    } else {
        TARGET = release/project
    }
}
win32:{
    TARGET = project
}

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo ../interpolation ../utilities


