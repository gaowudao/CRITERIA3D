#-------------------------------------------------
#
# CRITERIA3D distribution
# library Utilities
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

INCLUDEPATH += ../crit3dDate ../mathFunctions ../crop ../soil ../meteo ../gis

SOURCES += \
    utilities.cpp \
    dbTools.cpp \
    dbToolsMOSES.cpp

HEADERS += \
    utilities.h \
    dbTools.h \
    dbToolsMOSES.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
