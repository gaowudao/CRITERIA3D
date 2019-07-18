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
    LIBS += -L../../lib/weatherGenerator/release -lweatherGenerator
    LIBS += -L../../lib/weatherGenerator2D/release -lweatherGenerator2D
    LIBS += -L../../lib/specialMathFunctions/release -lspecialMathFunctions
    LIBS += -L../../lib/meteo/release -lmeteo
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
} else {
    LIBS += -L../../lib/weatherGenerator/debug -lweatherGenerator
    LIBS += -L../../lib/weatherGenerator2D/debug -lweatherGenerator2D
    LIBS += -L../../lib/specialMathFunctions/debug -lspecialMathFunctions  
    LIBS += -L../../lib/meteo/debug -lmeteo
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
}


INCLUDEPATH += ../../lib/weatherGenerator ../../lib/weatherGenerator2D ../../lib/mathFunctions ../../lib/specialMathFunctions \
                ../../lib/crit3dDate ../../lib/meteo ../../lib/gis


SOURCES += main.cpp \
    readPragaFormatData.cpp

HEADERS += \
    readPragaFormatData.h







