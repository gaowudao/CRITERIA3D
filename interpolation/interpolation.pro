#-------------------------------------------------
#
# CRITERIA3D
# interpolation library
#
#-------------------------------------------------

QT   -= gui

TARGET = interpolation
TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

#CONFIG(debug, debug|release) {
#    TARGET = debug/interpolation
#} else {
#    TARGET = release/interpolation
#}

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo

SOURCES += interpolation.cpp \
    interpolationSettings.cpp \
    interpolationPoint.cpp \
    kriging.cpp

HEADERS += interpolation.h \
    interpolationSettings.h \
    interpolationPoint.h \
    interpolationConstants.h \
    kriging.h

