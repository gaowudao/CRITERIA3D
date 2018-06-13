#-------------------------------------------------
#
# criteria3D library
# Soil Water Balance 3D
#
# this library is part of CRITERIA3D distribution
#
#-------------------------------------------------

QT      -= core gui

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/criteria3D
    } else {
        TARGET = release/criteria3D
    }
}
win32:{
    TARGET = criteria3D
}

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH +=  ../crit3dDate ../mathFunctions ../gis ../meteo \
                ../solarRadiation ../soil ../crop \
                ../soilFluxes3D/header


SOURCES += \
    waterBalance.cpp \
    crit3dProject.cpp \
    crit3dMeteoMaps.cpp \
    modelCore.cpp

HEADERS += \
    waterBalance.h \
    crit3dMeteoMaps.h \
    crit3dProject.h \
    modelCore.h
