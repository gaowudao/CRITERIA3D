#-----------------------------------------------------------------------------
#
# Criteria1D library
# Soil Water Balance 1D
# heuristic algorithms for soil water infiltration, redistribution and capillary rise
#
# this library is part of CRITERIA3D distribution
#
#-----------------------------------------------------------------------------

QT      += core sql
QT      -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/criteria1D
    } else {
        TARGET = release/criteria1D
    }
}
win32:{
    TARGET = criteria1D
}

INCLUDEPATH += ../crit3dDate ../mathFunctions ../utilities ../crop ../meteo ../soil ../gis

SOURCES += Criteria1D.cpp \
    modelCore.cpp \
    water1D.cpp \
    croppingSystem.cpp

HEADERS += Criteria1D.h \
    modelCore.h \
    water1D.h \
    croppingSystem.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


