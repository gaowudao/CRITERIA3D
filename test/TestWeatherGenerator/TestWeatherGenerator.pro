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

INCLUDEPATH += ../../agrolib/crit3dDate ../../agrolib/mathFunctions ../../agrolib/weatherGenerator

SOURCES += main.cpp \
    wgProject.cpp

CONFIG(debug, debug|release) {
    LIBS += -L../../agrolib/weatherGenerator/debug -lweatherGenerator
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate

} else {

    LIBS += -L../../agrolib/weatherGenerator/release -lweatherGenerator
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
}

HEADERS += \
    wgProject.h

