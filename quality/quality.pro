#-------------------------------------------------
#
# CRITERIA3D distribution
# quality library
#
#-------------------------------------------------

QT       -= gui

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

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += quality.cpp

HEADERS += quality.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
