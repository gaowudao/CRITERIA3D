#----------------------------------------------------
#
#   Crop Widget
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


INCLUDEPATH += ../crit3dDate ../mathFunctions ../utilities ../soil ../crop ../gis ../meteo

SOURCES += \
    cropDbTools.cpp \
    dbToolsMOSES.cpp

HEADERS += \
    cropDbTools.h \
    dbToolsMOSES.h



