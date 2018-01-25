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
LIBS += -L../mathFunctions/debug -lmathFunctions


SOURCES += wg2D.cpp \
    wg2d.cpp

HEADERS += wg2D.h
