#-------------------------------------------------
#
# Project created by QtCreator 2018-01-24T15:14:29
#
#-------------------------------------------------

QT       -= core gui

TARGET = weatherGenerator2D
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../mathFunctions
INCLUDEPATH += ../crit3dDate
INCLUDEPATH += ../meteo
INCLUDEPATH += ../gis
INCLUDEPATH += ../crit3dDate


SOURCES += wg2D.cpp \
    randomset.cpp \
    wg2D_precipitation.cpp \
    wg2D_temperature.cpp

HEADERS += wg2D.h
