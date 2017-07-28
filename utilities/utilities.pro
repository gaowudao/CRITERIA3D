#-------------------------------------------------
#
# utilities
# this library is part of CRITERIA3D
#
#-------------------------------------------------

QT      += core sql
QT      -= gui

TARGET = utilities
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions ../crop ../soil ../meteo ../gis

SOURCES += utilities.cpp \
    dbTools.cpp \
    dbToolsMOSES.cpp

HEADERS += utilities.h \
    dbTools.h \
    dbToolsMOSES.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
