#-------------------------------------------------
#
# CRITERIA3D
# meteo library
#
#-------------------------------------------------

QT       -= core gui

TARGET = meteo
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis

SOURCES += meteo.cpp \
    meteoPoint.cpp

HEADERS += meteo.h \
    meteoPoint.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
