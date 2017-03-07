#-------------------------------------------------
#
# Project created by QtCreator 2017-03-02T16:07:56
#
#-------------------------------------------------

QT       += network sql

QT       -= gui

TARGET = dbMeteoPoints

#TEMPLATE = lib
#CONFIG += staticlib
#DEFINES += DBMETEOPOINTS_LIBRARY

TEMPLATE = app #solo x test

INCLUDEPATH += ../mathFunctions ../meteo

SOURCES += dbMeteoPoints.cpp \
    download.cpp \
    main.cpp

HEADERS += dbMeteoPoints.h \
    download.h


