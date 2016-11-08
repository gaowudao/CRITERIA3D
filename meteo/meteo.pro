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

INCLUDEPATH += ../crit3dDate
INCLUDEPATH += ../mathFunctions
INCLUDEPATH += ../gis

SOURCES += meteo.cpp \
    quality.cpp \
    meteoPoint.cpp

HEADERS += meteo.h \
    meteoPoint.h \
    quality.h


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
