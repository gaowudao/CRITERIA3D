#-------------------------------------------------------------------
#
# Weather generator test
# this project is part of CRITERIA3D distribution
#
#-------------------------------------------------------------------


TARGET = WG

QT += core xml
QT -= gui

CONFIG += c++11

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../weatherGenerator

SOURCES += main.cpp

CONFIG(debug, debug|release) {
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../weatherGenerator/debug -lweatherGenerator
} else {
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
    LIBS += -L../weatherGenerator/release -lweatherGenerator
}

