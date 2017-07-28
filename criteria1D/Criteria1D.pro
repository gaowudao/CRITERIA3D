#-----------------------------------------------------------------------------
#
# Criteria1D
# Soil Water Balance 1D
# with heuristic algorithms for soil water infiltration, redistribution and capillary rise
#
# this library is part of CRITERIA3D
#
#-----------------------------------------------------------------------------

QT      += core sql
QT      -= gui

TARGET = Criteria1D
TEMPLATE = lib
CONFIG += staticlib

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


