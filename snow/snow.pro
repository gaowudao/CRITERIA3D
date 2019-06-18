#----------------------------------------------------
#
#   SNOW
#   Snow accumulation and melt library
#   This library is part of CRITERIA-3D distribution
#
#----------------------------------------------------

QT += core
QT -= gui


TARGET = snow
CONFIG += staticlib

TEMPLATE = lib

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo ../solarRadiation

SOURCES += \
    snowMaps.cpp \
    snowPoint.cpp

HEADERS += \
    snowMaps.h \
    snowPoint.h
