#---------------------------------------------------------
#
#   testSunPosition
#   show current sunrise and sunset at Bologna and Madrid
#   this project is part of CRITERIA3D distribution
#
#---------------------------------------------------------

QT       += core
QT       -= gui

TARGET = testSunPosition
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
