#------------------------------------------------------------------
#
#   criteriaModel library
#   Mono-dimensional water balance.
#   Algorithms for soil water infiltration, redistribution,
#   capillary rise, crop water demand and irrigation.
#
#   This library is part of CRITERIA3D distribution
#
#------------------------------------------------------------------

QT      += core sql
QT      -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/criteriaModel
    } else {
        TARGET = release/criteriaModel
    }
}
win32:{
    TARGET = criteriaModel
}

INCLUDEPATH +=  ../crit3dDate ../mathFunctions ../soil ../crop ../gis ../meteo \
                ../utilities ../cropWidget

SOURCES += \
    modelCore.cpp \
    water1D.cpp \
    croppingSystem.cpp \
    criteriaModel.cpp \
    dbToolsMOSES.cpp

HEADERS += \
    modelCore.h \
    water1D.h \
    croppingSystem.h \
    criteriaModel.h \
    dbToolsMOSES.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

