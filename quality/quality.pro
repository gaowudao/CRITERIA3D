#-------------------------------------------------
#
# CRITERIA3D distribution
# quality library
#
#-------------------------------------------------

QT       -= gui core

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/quality
    } else {
        TARGET = release/quality
    }
}
win32:{
    TARGET = quality
}

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo ../interpolation

SOURCES += quality.cpp

HEADERS += quality.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
