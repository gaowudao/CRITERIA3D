#------------------------------------------------------------
#
#   TestSolarRadiation
#   It computes a map of global solar irradiance (clear sky)
#   for a specified date/time, starting from a DTM
#
#   This project is part of CRITERIA3D distribution
#
#------------------------------------------------------------


QT       -= gui

TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestSolarRadiation
    } else {
        TARGET = release/TestSolarRadiation
    }
}
win32:{
    TARGET = TestSolarRadiation
}

INCLUDEPATH += ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/gis ../../lib/solarRadiation

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/solarRadiation/debug -lsolarRadiation
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
} else {
    LIBS += -L../../lib/solarRadiation/release -lsolarRadiation
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
}

SOURCES += main.cpp
