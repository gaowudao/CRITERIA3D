#-------------------------------------------------
#
#   Utilities library
#   This project is part of CRITERIA-3D distribution
#
#-------------------------------------------------

QT      += core sql
QT      -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/utilities
    } else {
        TARGET = release/utilities
    }
}
win32:{
    TARGET = utilities
}

INCLUDEPATH += ../crit3dDate ../mathFunctions

SOURCES += \
    utilities.cpp

HEADERS += \
    utilities.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}