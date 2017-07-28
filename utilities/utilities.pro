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

INCLUDEPATH += ../crit3dDate ../mathFunctions ../crop ../soil

SOURCES += utilities.cpp \
    dbTools.cpp

HEADERS += utilities.h \
    dbTools.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
