#-------------------------------------------------
#
# Project created by QtCreator 2016-03-21T16:11:17
#
#-------------------------------------------------

QT      += core sql
QT      -= gui

TARGET = Criteria1D
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions ../crop ../meteo ../soil ../gis

SOURCES += Criteria1D.cpp \
    modelCore.cpp \
    dataHandler.cpp \
    water1D.cpp \
    croppingSystem.cpp

HEADERS += Criteria1D.h \
    dataHandler.h \
    modelCore.h \
    water1D.h \
    croppingSystem.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


