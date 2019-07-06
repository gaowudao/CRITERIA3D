#-----------------------------------------------------------------
#
#   TestSunPosition
#   It shows the current sunrise and sunset at Bologna and Madrid
#
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------------------

QT       += core
QT       -= gui

TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestSunPosition
    } else {
        TARGET = release/TestSunPosition
    }
}
win32:{
    TARGET = TestSunPosition
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
