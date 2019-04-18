#---------------------------------------------------------
#
#   testSolarRadiation
#   compute a map of global solar irradiance (clear sky)
#   for a specified date/time, starting from a DTM
#
#   this project is part of CRITERIA3D distribution
#
#---------------------------------------------------------


QT       -= gui

TARGET = testSolarRadiation
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += ../solarRadiation ../gis ../crit3dDate ../mathFunctions

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    LIBS += -L../solarRadiation/debug -lsolarRadiation
    LIBS += -L../gis/debug -lgis
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../solarRadiation/release -lsolarRadiation
    LIBS += -L../gis/release -lgis
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
}

TEMPLATE = app

SOURCES += main.cpp
