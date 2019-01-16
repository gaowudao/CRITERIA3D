#-----------------------------------------------
#
# grapevine library
#
#-----------------------------------------------

QT      -= core gui

TARGET = grapevine
TEMPLATE = lib
CONFIG += staticlib

DEFINES += GRAPEVINE_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/grapevine
    } else {
        TARGET = release/grapevine
    }
}
win32:{
    TARGET = grapevine
}

INCLUDEPATH += ../mathFunctions ../crit3dDate ../soil

SOURCES += grapevine.cpp

HEADERS += grapevine.h

