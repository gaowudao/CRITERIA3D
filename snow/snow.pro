QT += core
QT -= gui

CONFIG += c++11

TARGET = snow
CONFIG += staticlib

TEMPLATE = lib

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../MapGraphics ../meteo ../solarRadiation

LIBS += -L../mathFunctions/debug -lmathFunctions
LIBS += -L../gis/debug -lgis
LIBS += -L../meteo/release -lmeteo
LIBS += -L../solarRadiation/release -lsolarRadiation

SOURCES += \
    snowMaps.cpp \
    snowPoint.cpp

HEADERS += \
    snowparam.h \
    snowMaps.h \
    snowPoint.h
