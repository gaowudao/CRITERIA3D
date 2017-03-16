#-------------------------------------------------
#
# Project created by QtCreator 2017-03-02T16:07:56
#
#-------------------------------------------------

#QT       += network sql

#QT       -= gui

QT       += core gui sql widgets network #solo x test

TARGET = dbMeteoPoints

#TEMPLATE = lib
#CONFIG += staticlib
#DEFINES += DBMETEOPOINTS_LIBRARY

TEMPLATE = app #solo x test

INCLUDEPATH += ../mathFunctions ../gis ../meteo ../crit3dDate

LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../gis/release -lgis
LIBS += -L../crit3dDate/release -lcrit3dDate

SOURCES += dbMeteoPoints.cpp \
    download.cpp \
    mainwindow.cpp \
    main.cpp

HEADERS += dbMeteoPoints.h \
    download.h \
    mainwindow.h


