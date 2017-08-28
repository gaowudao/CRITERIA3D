#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T15:35:21
#
#-------------------------------------------------

QT       += core gui widgets network sql

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../utilities ../gis ../MapGraphics ../meteo ../quality ../interpolation ../dbMeteoPoints

LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../crit3dDate/release -lcrit3dDate
LIBS += -L../utilities/release -lutilities
LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
LIBS += -L../meteo/release -lmeteo
LIBS += -L../gis/release -lgis
LIBS += -L../quality/release -lquality
LIBS += -L../interpolation/release -linterpolation
LIBS += -L../MapGraphics/release -lMapGraphics

INCLUDEPATH += $$(NC4_INSTALL_DIR)/include
LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf -lhdf5


SOURCES += main.cpp\
    mainwindow.cpp \
    rasterObject.cpp \
    colorlegend.cpp \
    project.cpp \
    stationMarker.cpp \
    rubberBand.cpp \
    formInfo.cpp \
    dialogWindows.cpp \
    netcdfManager.cpp

HEADERS  += mainwindow.h \
    rasterObject.h \
    colorlegend.h \
    project.h \
    guiConfiguration.h \
    stationMarker.h \
    rubberBand.h \
    formInfo.h \
    dialogWindows.h \
    netcdfManager.h

FORMS    += mainwindow.ui \
    formInfo.ui

DISTFILES +=
