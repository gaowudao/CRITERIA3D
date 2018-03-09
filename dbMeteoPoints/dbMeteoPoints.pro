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

SOURCES += \
    download.cpp \
    variableslist.cpp \
    dbArkimet.cpp \
    dbMeteoPoints.cpp

HEADERS += \
    download.h \
    variableslist.h \
    dbArkimet.h \
    dbMeteoPoints.h


