QT += core
QT -= gui

TARGET = TestWG2D
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../weatherGenerator2D
INCLUDEPATH += ../mathFunctions
INCLUDEPATH += ../crit3dDate
INCLUDEPATH += ../meteo
INCLUDEPATH += ../gis
CONFIG(release, debug|release) {
    LIBS += -L../weatherGenerator2D/release -lweatherGenerator2D
    LIBS += -L../mathFunctions/release -lmathFunctions
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
} else {
    LIBS += -L../weatherGenerator2D/debug -lweatherGenerator2D
    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
}

TEMPLATE = app

SOURCES += main.cpp

CONFIG += console
CONFIG -= app_bundle





