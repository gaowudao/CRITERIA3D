#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T15:35:21
#
#-------------------------------------------------

QT       += core gui widgets network sql

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../MapGraphics ../meteo ../dbMeteoPoints

LIBS += -L../dbMeteoPoints/debug -ldbMeteoPoints
LIBS += -L../meteo/debug -lmeteo
LIBS += -L../gis/debug -lgis
LIBS += -L../mathFunctions/debug -lmathFunctions
LIBS += -L../crit3dDate/debug -lcrit3dDate
LIBS += -L../MapGraphics/debug -lMapGraphics


SOURCES += main.cpp\
    mainwindow.cpp \
    rasterObject.cpp \
    colorlegend.cpp \
    formSingleValue.cpp \
    project.cpp \
    stationMarker.cpp \
    rubberBand.cpp \
    utils.cpp

HEADERS  += mainwindow.h \
    rasterObject.h \
    colorlegend.h \
    formSingleValue.h \
    project.h \
    guiConfiguration.h \
    stationMarker.h \
    rubberBand.h \
    utils.h

FORMS    += mainwindow.ui \
    formSingleValue.ui

DISTFILES +=
