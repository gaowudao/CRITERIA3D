#-------------------------------------------------
#
# CRITERIA3D distribution
# solarRadiation library
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/solarRadiation
    } else {
        TARGET = release/solarRadiation
    }
}
win32:{
    TARGET = solarRadiation
}

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo

SOURCES += \
    solPos.cpp \
    solarRadiation.cpp \
    sunPosition.cpp \
    radiationSettings.cpp \
    transmissivity.cpp

HEADERS += \
    solPos.h \
    sunPosition.h \
    radiationSettings.h \
    radiationDefinitions.h \
    solarRadiation.h \
    transmissivity.h

