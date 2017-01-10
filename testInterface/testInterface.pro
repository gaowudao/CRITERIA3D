#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T15:35:21
#
#-------------------------------------------------

QT       += core gui widgets network

TARGET = testInterface
TEMPLATE = app

INCLUDEPATH += ../mathFunctions ../gis ../MapGraphics

LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../gis/release -lgis
LIBS += -L../MapGraphics/release -lMapGraphics

SOURCES += main.cpp\
    mainwindow.cpp \
    RasterObject.cpp \
    raster.cpp \
    colorlegend.cpp

HEADERS  += mainwindow.h \
    RasterObject.h \
    raster.h \
    colorlegend.h

FORMS    += mainwindow.ui

DISTFILES +=
