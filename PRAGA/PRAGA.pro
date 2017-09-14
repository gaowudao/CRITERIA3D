#-------------------------------------------------
#
# CRITERIA3D distribution
# PRAGA (PRogram for AGrometeorological Analysis)
#
#-------------------------------------------------

QT       += core gui widgets network sql

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../utilities ../gis ../MapGraphics ../meteo ../quality ../interpolation ../dbMeteoPoints ../netcdfHandler

LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../crit3dDate/release -lcrit3dDate
LIBS += -L../utilities/release -lutilities
LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
LIBS += -L../meteo/release -lmeteo
LIBS += -L../gis/release -lgis
LIBS += -L../quality/release -lquality
LIBS += -L../interpolation/release -linterpolation
LIBS += -L../MapGraphics/release -lMapGraphics
LIBS += -L../netcdfHandler/release -lnetcdfHandler

LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf -lhdf5


SOURCES += main.cpp\
    mainwindow.cpp \
    rasterObject.cpp \
    colorlegend.cpp \
    project.cpp \
    stationMarker.cpp \
    rubberBand.cpp \
    formInfo.cpp \
    dialogWindows.cpp

HEADERS  += mainwindow.h \
    rasterObject.h \
    colorlegend.h \
    project.h \
    guiConfiguration.h \
    stationMarker.h \
    rubberBand.h \
    formInfo.h \
    dialogWindows.h

FORMS    += mainwindow.ui \
    formInfo.ui

