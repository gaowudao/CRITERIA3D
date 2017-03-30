#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T15:35:21
#
#-------------------------------------------------

QT       += core gui widgets network sql

TARGET = testInterface
TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../MapGraphics ../dbMeteoPoints

LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../gis/release -lgis
LIBS += -L../MapGraphics/release -lMapGraphics
LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
LIBS += -L../crit3dDate/release -lcrit3dDate

SOURCES += main.cpp\
    mainwindow.cpp \
    RasterObject.cpp \
    colorlegend.cpp \
    formSingleValue.cpp \
    project.cpp

HEADERS  += mainwindow.h \
    RasterObject.h \
    colorlegend.h \
    formSingleValue.h \
    project.h

FORMS    += mainwindow.ui \
    formSingleValue.ui

DISTFILES +=
