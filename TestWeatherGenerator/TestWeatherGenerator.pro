#-------------------------------------------------------------------
#
#   TESTWEATHERGENERATOR
#   1D Weather Generator test
#   this project is part of CRITERIA3D distribution
#
#-------------------------------------------------------------------


QT += core xml
QT -= gui

CONFIG += c++11

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/WG
    } else {
        TARGET = release/WG
    }
}
win32:{
    TARGET = WG
}

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

