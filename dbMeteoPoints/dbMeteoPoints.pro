#-------------------------------------------------
#
# Project created by QtCreator 2017-03-02T16:07:56
#
#-------------------------------------------------

QT       += network sql

QT       -= gui

TARGET = dbMeteoPoints

TEMPLATE = lib
CONFIG += staticlib
DEFINES += DBMETEOPOINTS_LIBRARY

INCLUDEPATH += ../mathFunctions ../gis ../meteo ../crit3dDate

LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../gis/release -lgis
LIBS += -L../crit3dDate/release -lcrit3dDate

SOURCES += dbMeteoPoints.cpp \
    download.cpp \
    variableslist.cpp

HEADERS += dbMeteoPoints.h \
    download.h \
    variableslist.h


