#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T15:35:21
#
#-------------------------------------------------

QT       += core gui widgets network sql

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../MapGraphics ../meteo ../dbMeteoPoints

LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
LIBS += -L../meteo/release -lmeteo
LIBS += -L../gis/release -lgis
LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../crit3dDate/release -lcrit3dDate
LIBS += -L../MapGraphics/release -lMapGraphics


SOURCES += main.cpp\
    mainwindow.cpp \
    rasterObject.cpp \
    colorlegend.cpp \
    formSingleValue.cpp \
    project.cpp \
    stationMarker.cpp \
    formDownload.cpp

HEADERS  += mainwindow.h \
    rasterObject.h \
    colorlegend.h \
    formSingleValue.h \
    project.h \
    guiConfiguration.h \
    stationMarker.h \
    formDownload.h

FORMS    += mainwindow.ui \
    formSingleValue.ui \
    formDownload.ui

DISTFILES +=
