#-------------------------------------------------------------------
#
#   TestWeatherGenerator
#   Weather Generator (1D) test
#
#   This project is part of CRITERIA3D distribution
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

INCLUDEPATH += ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/weatherGenerator

SOURCES += main.cpp

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/weatherGenerator/debug -lweatherGenerator
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate

} else {

    LIBS += -L../../lib/weatherGenerator/release -lweatherGenerator
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
}

