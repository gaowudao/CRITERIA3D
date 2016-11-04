#-------------------------------------------------
#
# CRITERIA3D
# gis library
#
#-------------------------------------------------

QT       -= core gui

TARGET = gis
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../mathFunctions

SOURCES += gis.cpp \
    gisIO.cpp \
    color.cpp \
    map.cpp

HEADERS += gis.h \
    color.h \
    gisIO.h \
    map.h
