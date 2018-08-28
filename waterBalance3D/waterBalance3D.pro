#-------------------------------------------------
#
# waterBalance3D library
# Soil Water Balance 3D
#
# this library is part of CRITERIA3D distribution
#
#-------------------------------------------------

QT      -= core gui

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/waterBalance3D
    } else {
        TARGET = release/waterBalance3D
    }
}
win32:{
    TARGET = waterBalance3D
}

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH +=  ../crit3dDate ../mathFunctions ../gis ../meteo \
                ../solarRadiation ../soil \
                ../soilFluxes3D/header


SOURCES += \
    crit3dMeteoMaps.cpp \
    crit3dProject.cpp \
    waterBalance3D.cpp \
    modelCore3D.cpp

HEADERS += \
    crit3dMeteoMaps.h \
    crit3dProject.h \
    waterBalance3D.h \
    modelCore3D.h
