#-------------------------------------------------
#
# CRITERIA3D distribution
# soil library
#
#-------------------------------------------------

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


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
