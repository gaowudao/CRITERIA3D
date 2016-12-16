#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T15:35:21
#
#-------------------------------------------------

QT       += core gui widgets network

TARGET = testInterface
TEMPLATE = app

INCLUDEPATH += ../mathFunctions ../gis ../MapGraphics

LIBS += -L../mathFunctions/debug -lmathFunctions
LIBS += -L../gis/debug -lgis
LIBS += -L../MapGraphics/debug -lMapGraphics

SOURCES += main.cpp\
        mainwindow.cpp \
    raster.cpp \
    RasterObject.cpp

HEADERS  += mainwindow.h \
    raster.h \
    RasterObject.h

FORMS    += mainwindow.ui

DISTFILES +=
