#-------------------------------------------------------------------
#
#   TestWG2D
#   Weather Generator (2D) test
#
#   This project is part of CRITERIA3D distribution
#
#-------------------------------------------------------------------

QT += xml
QT -= gui

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

DEFINES += _CRT_SECURE_NO_WARNINGS

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestWG2D
    } else {
        TARGET = release/TestWG2D
    }
}
win32:{
    TARGET = TestWG2D
}


CONFIG(release, debug|release) {
    LIBS += -L../../agrolib/weatherGenerator2D/release -lweatherGenerator2D
    LIBS += -L../../agrolib/weatherGenerator/release -lweatherGenerator
    LIBS += -L../../agrolib/eispack/release -leispack
    LIBS += -L../../agrolib/meteo/release -lmeteo
    LIBS += -L../../agrolib/gis/release -lgis
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
} else {
    LIBS += -L../../agrolib/weatherGenerator2D/debug -lweatherGenerator2D
    LIBS += -L../../agrolib/weatherGenerator/debug -lweatherGenerator
    LIBS += -L../../agrolib/eispack/debug -leispack
    LIBS += -L../../agrolib/meteo/debug -lmeteo
    LIBS += -L../../agrolib/gis/debug -lgis
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions
}


INCLUDEPATH += ../../agrolib/weatherGenerator ../../agrolib/weatherGenerator2D ../../agrolib/mathFunctions ../../agrolib/eispack \
                ../../agrolib/crit3dDate ../../agrolib/meteo ../../agrolib/gis


SOURCES += main.cpp \
    readPragaFormatData.cpp

HEADERS += \
    readPragaFormatData.h







