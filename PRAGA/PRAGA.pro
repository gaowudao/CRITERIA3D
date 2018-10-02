#-----------------------------------------------------
#
# PRAGA
# PRogram for AGrometeorological Analysis
#
# this software is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += core gui widgets network sql xml

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH +=  ./shared ../MapGraphics \
                ../crit3dDate ../mathFunctions ../meteo ../gis ../utilities \
                ../interpolation ../solarRadiation ../dbMeteoPoints ../dbMeteoGrid ../climate

CONFIG += debug_and_release

unix:{
    LIBS += -L../MapGraphics/release -lMapGraphics
}
win32:{
    CONFIG(debug, debug|release) {
         LIBS += -L../MapGraphics/debug -lMapGraphics
    } else {
        LIBS += -L../MapGraphics/release -lMapGraphics
    }
}

CONFIG(debug, debug|release) {
    LIBS += -L../climate/debug -lclimate
    LIBS += -L../dbMeteoGrid/debug -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/debug -ldbMeteoPoints
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../solarRadiation/debug -lsolarRadiation
    LIBS += -L../interpolation/debug -linterpolation
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../climate/release -lclimate
    LIBS += -L../dbMeteoGrid/release -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../solarRadiation/release -lsolarRadiation
    LIBS += -L../interpolation/release -linterpolation
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
}


#ifdef NETCDF
#LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf


SOURCES += main.cpp\
    mainwindow.cpp \
    project.cpp \
    computationDialog.cpp \
    anomalyLayout.cpp \
    saveClimaLayout.cpp \
    shared/dialogWindows.cpp \
    shared/stationMarker.cpp \
    shared/rubberBand.cpp \
    shared/formRunInfo.cpp \
    shared/settingsDialog.cpp \
    shared/rasterObject.cpp \
    shared/colorlegend.cpp \
    shared/interpolationDialog.cpp \
    shared/interpolationCmd.cpp

HEADERS  += mainwindow.h \
    project.h \
    saveClimaLayout.h \
    computationDialog.h \
    anomalyLayout.h \
    shared/dialogWindows.h \
    shared/stationMarker.h \
    shared/rubberBand.h \
    shared/formRunInfo.h \
    shared/settingsDialog.h \
    shared/rasterObject.h \
    shared/colorlegend.h \
    shared/interpolationDialog.h \
    shared/interpolationCmd.h

FORMS    += mainwindow.ui \
        shared/formInfo.ui

