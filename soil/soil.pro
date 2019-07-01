#----------------------------------------------------
#
#   Soil library
#   This project is part of CRITERIA-3D distribution
#
#----------------------------------------------------

QT  -= core gui

TARGET = soil
TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/soil
    } else {
        TARGET = release/soil
    }
}
win32:{
    TARGET = soil
}

INCLUDEPATH += ../mathFunctions

SOURCES += soil.cpp

HEADERS += soil.h

