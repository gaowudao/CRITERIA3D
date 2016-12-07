QT += core
QT -= gui

CONFIG += c++11

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
