#-------------------------------------------------
#
# crit3dDate library
# this library is part of CRITERIA3D distribution
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/crit3dDate
    } else {
        TARGET = release/crit3dDate
    }
}
win32:{
    TARGET = crit3dDate
}

INCLUDEPATH += ../mathFunctions

SOURCES += \
    crit3dDate.cpp \
    crit3dTime.cpp

HEADERS += \
    crit3dDate.h



