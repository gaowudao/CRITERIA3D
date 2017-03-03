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

INCLUDEPATH += ../mathFunctions ../meteo

SOURCES += dbMeteoPoints.cpp \
    download.cpp

HEADERS += dbMeteoPoints.h \
    download.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
