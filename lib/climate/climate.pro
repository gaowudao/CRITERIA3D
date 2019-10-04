#-----------------------------------------------------
#
#   climate library
#   This project is part of CRITERIA-3D distribution
#
#-----------------------------------------------------

QT       -= gui
QT       += sql xml

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


INCLUDEPATH +=  ../crit3dDate ../mathFunctions ../gis ../meteo ../interpolation \
                ../utilities ../dbMeteoPoints ../dbMeteoGrid

SOURCES += \
    climate.cpp \
    elaborationSettings.cpp \
    crit3dClimate.cpp \
    dbClimate.cpp \
    crit3dClimateList.cpp \
    crit3dElabList.cpp

HEADERS += \
    dbClimate.h \
    climate.h \
    elaborationSettings.h \
    crit3dClimate.h \
    crit3dClimateList.h \
    crit3dElabList.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
