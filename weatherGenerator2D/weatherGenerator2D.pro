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



SOURCES += wg2D.cpp

HEADERS += wg2D.h
