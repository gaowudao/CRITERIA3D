#-------------------------------------------------
#
# Project created by QtCreator 2018-06-11T10:39:18
#
#-------------------------------------------------

QT       -= gui

TARGET = percentileAnalysis
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../mathFunctions
INCLUDEPATH += ../crit3dDate


SOURCES += percentileanalysis.cpp

HEADERS += percentileanalysis.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

