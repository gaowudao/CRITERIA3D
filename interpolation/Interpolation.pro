#-------------------------------------------------
#
# CRITERIA3D
# interpolation library
#
#-------------------------------------------------

QT   -= gui

TARGET = Interpolation
TEMPLATE = lib
CONFIG += staticlib

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

