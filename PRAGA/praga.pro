#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T15:35:21
#
#-------------------------------------------------

QT       += core gui widgets network sql

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../MapGraphics ../meteo ../dbMeteoPoints

LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../gis/release -lgis
LIBS += -L../MapGraphics/release -lMapGraphics
LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
LIBS += -L../crit3dDate/release -lcrit3dDate
LIBS += -L../meteo/release -lmeteo

SOURCES += main.cpp\
    mainwindow.cpp \
    rasterObject.cpp \
    colorlegend.cpp \
    formSingleValue.cpp \
    project.cpp \
    stationMarker.cpp

HEADERS  += mainwindow.h \
    rasterObject.h \
    colorlegend.h \
    formSingleValue.h \
    project.h \
    guiConfiguration.h \
    stationMarker.h

FORMS    += mainwindow.ui \
    formSingleValue.ui

DISTFILES +=
