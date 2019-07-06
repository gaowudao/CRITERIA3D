#-------------------------------------------------------------------
#
#   TestWG2D
#   Weather Generator (2D) test
#
#   This project is part of CRITERIA3D distribution
#
#-------------------------------------------------------------------

QT += core
QT -= gui

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

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
    LIBS += -L../../lib/weatherGenerator2D/release -lweatherGenerator2D
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
    LIBS += -L../../lib/specialMathFunctions/release -lspecialMathFunctions
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../lib/meteo/release -lmeteo
    LIBS += -L../../lib/gis/release -lgis
} else {
    LIBS += -L../../lib/weatherGenerator2D/debug -lweatherGenerator2D
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../lib/specialMathFunctions/debug -lspecialMathFunctions
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../lib/meteo/debug -lmeteo
    LIBS += -L../../lib/gis/debug -lgis
}


INCLUDEPATH += ../../lib/weatherGenerator2D ../../lib/mathFunctions ../../lib/specialMathFunctions \
                ../../lib/crit3dDate ../../lib/meteo ../../lib/gis


SOURCES += main.cpp \
    inputData.cpp \
    readPragaFormatData.cpp

HEADERS += \
    inputData.h \
    readPragaFormatData.h







