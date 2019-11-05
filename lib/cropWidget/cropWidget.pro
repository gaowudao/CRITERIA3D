#----------------------------------------------------
#
#   Crop Widget library
#   This project is part of CRITERIA-3D distribution
#
#----------------------------------------------------

QT  += widgets sql

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/cropWidget
    } else {
        TARGET = release/cropWidget
    }
}
win32:{
    TARGET = cropWidget
}


INCLUDEPATH += ../crit3dDate ../mathFunctions ../meteo ../soil ../crop ../utilities

SOURCES += \
    cropDbTools.cpp

HEADERS += \
    cropDbTools.h



