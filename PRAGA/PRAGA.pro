#-------------------------------------------------
#
# CRITERIA3D distribution
# PRAGA (PRogram for AGrometeorological Analysis)
#
#-------------------------------------------------

QT       += core gui widgets network sql

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH +=  ../crit3dDate ../mathFunctions ../utilities ../gis ../MapGraphics \
                ../meteo ../quality ../interpolation ../solarRadiation \
                ../dbMeteoPoints ../netcdfHandler

LIBS += -L../dbMeteoPoints/debug -ldbMeteoPoints
LIBS += -L../meteo/debug -lmeteo
LIBS += -L../interpolation/debug -linterpolation
LIBS += -L../solarRadiation/debug -lsolarRadiation
LIBS += -L../MapGraphics/release -lMapGraphics
LIBS += -L../gis/debug -lgis
LIBS += -L../netcdfHandler/debug -lnetcdfHandler
LIBS += -L../mathFunctions/debug -lmathFunctions
LIBS += -L../crit3dDate/debug -lcrit3dDate
LIBS += -L../utilities/debug -lutilities

LIBS += -L../quality/debug -lquality


#LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf -lhdf5
LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf



SOURCES += main.cpp\
    mainwindow.cpp \
    rasterObject.cpp \
    colorlegend.cpp \
    project.cpp \
    stationMarker.cpp \
    rubberBand.cpp \
    dialogWindows.cpp \
    formRunInfo.cpp

HEADERS  += mainwindow.h \
    rasterObject.h \
    colorlegend.h \
    project.h \
    guiConfiguration.h \
    stationMarker.h \
    rubberBand.h \
    dialogWindows.h \
    formRunInfo.h

FORMS    += mainwindow.ui \
    formInfo.ui

