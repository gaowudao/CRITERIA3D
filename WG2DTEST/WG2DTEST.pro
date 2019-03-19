QT += core
QT -= gui

TARGET = WG2DTEST
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../weatherGenerator2D
INCLUDEPATH += ../mathFunctions
INCLUDEPATH += ../crit3dDate
INCLUDEPATH += ../meteo
INCLUDEPATH += ../gis

LIBS += -L../weatherGenerator2D/release -lweatherGenerator2D
LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../crit3dDate/release -lcrit3dDate
LIBS += -L../meteo/release -lmeteo
LIBS += -L../gis/release -lgis


TEMPLATE = app

SOURCES += main.cpp

CONFIG += console
CONFIG -= app_bundle





