#-------------------------------------------------
#
# CRITERIA3D distribution
# climate library
#
#-------------------------------------------------

QT       -= gui
QT       += core

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release


unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/climate
    } else {
        TARGET = release/climate
    }
}
win32:{
    TARGET = climate
}

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo

SOURCES += \
        climate.cpp

HEADERS += \
        climate.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
