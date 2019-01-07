#-------------------------------------------------
#
# CRITERIA3D distribution
# snow accumulation and melt library
#
#-------------------------------------------------

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
