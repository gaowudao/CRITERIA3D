#-------------------------------------------------
#
# utilities
# this library is part of CRITERIA3D
#
#-------------------------------------------------

QT       -= gui

TARGET = utilities
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions

SOURCES += utilities.cpp

HEADERS += utilities.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
