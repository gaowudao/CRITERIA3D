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

    PRE_TARGETDEPS += ../climate/debug/libclimate.a
    LIBS += -L../climate/debug -lclimate

    PRE_TARGETDEPS += ../dbMeteoGrid/debug/libdbMeteoGrid.a
    LIBS += -L../dbMeteoGrid/debug -ldbMeteoGrid

    PRE_TARGETDEPS += ../dbMeteoPoints/debug/libdbMeteoPoints.a
    LIBS += -L../dbMeteoPoints/debug -ldbMeteoPoints

    PRE_TARGETDEPS += ../utilities/debug/libutilities.a
    LIBS += -L../utilities/debug -lutilities

    PRE_TARGETDEPS += ../solarRadiation/debug/libsolarRadiation.a
    LIBS += -L../solarRadiation/debug -lsolarRadiation

    PRE_TARGETDEPS += ../interpolation/debug/libinterpolation.a
    LIBS += -L../interpolation/debug -linterpolation

    PRE_TARGETDEPS += ../meteo/debug/libmeteo.a
    LIBS += -L../meteo/debug -lmeteo

    PRE_TARGETDEPS += ../gis/debug/libgis.a
    LIBS += -L../gis/debug -lgis

    PRE_TARGETDEPS += ../crit3dDate/debug/libcrit3dDate.a
    LIBS += -L../crit3dDate/debug -lcrit3dDate

    PRE_TARGETDEPS += ../mathFunctions/debug/libmathFunctions.a
    LIBS += -L../mathFunctions/debug -lmathFunctions
} else {
    PRE_TARGETDEPS += ../climate/release/libclimate.a
    LIBS += -L../climate/release -lclimate

    PRE_TARGETDEPS += ../dbMeteoGrid/release/libdbMeteoGrid.a
    LIBS += -L../dbMeteoGrid/release -ldbMeteoGrid

    PRE_TARGETDEPS += ../dbMeteoPoints/release/libdbMeteoPoints.a
    LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints

    PRE_TARGETDEPS += ../utilities/release/libutilities.a
    LIBS += -L../utilities/release -lutilities

    PRE_TARGETDEPS += ../solarRadiation/release/libsolarRadiation.a
    LIBS += -L../solarRadiation/release -lsolarRadiation

    PRE_TARGETDEPS += ../interpolation/release/libinterpolation.a
    LIBS += -L../interpolation/release -linterpolation

    PRE_TARGETDEPS += ../meteo/release/libmeteo.a
    LIBS += -L../meteo/release -lmeteo

    PRE_TARGETDEPS += ../gis/release/libgis.a
    LIBS += -L../gis/release -lgis

    PRE_TARGETDEPS += ../crit3dDate/release/libcrit3dDate.a
    LIBS += -L../crit3dDate/release -lcrit3dDate

    PRE_TARGETDEPS += ../mathFunctions/release/libmathFunctions.a
    LIBS += -L../mathFunctions/release -lmathFunctions
}


#ifdef NETCDF
#LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf


SOURCES += main.cpp\
    mainwindow.cpp \
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
    shared/interpolationCmd.cpp \
    shared/project.cpp \
    pragaProject.cpp

HEADERS  += mainwindow.h \
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
    shared/interpolationCmd.h \
    shared/project.h \
    pragaProject.h

FORMS    += mainwindow.ui \
        shared/formInfo.ui

