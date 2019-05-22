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

INCLUDEPATH +=  ./shared ../mapGraphics \
                ../crit3dDate ../mathFunctions ../meteo ../gis ../utilities \
                ../interpolation ../solarRadiation ../dbMeteoPoints ../dbMeteoGrid ../climate

CONFIG += debug_and_release

unix:{
    LIBS += -L../mapGraphics/release -lMapGraphics
}
win32:{
    CONFIG(debug, debug|release) {
         LIBS += -L../mapGraphics/debug -lMapGraphics
    } else {
        LIBS += -L../mapGraphics/release -lMapGraphics
    }
}

CONFIG(debug, debug|release) {

unix:{
    PRE_TARGETDEPS += ../climate/debug/libclimate.a
    PRE_TARGETDEPS += ../dbMeteoGrid/debug/libdbMeteoGrid.a
    PRE_TARGETDEPS += ../dbMeteoPoints/debug/libdbMeteoPoints.a
    PRE_TARGETDEPS += ../utilities/debug/libutilities.a
    PRE_TARGETDEPS += ../solarRadiation/debug/libsolarRadiation.a
    PRE_TARGETDEPS += ../interpolation/debug/libinterpolation.a
    PRE_TARGETDEPS += ../meteo/debug/libmeteo.a
    PRE_TARGETDEPS += ../gis/debug/libgis.a
    PRE_TARGETDEPS += ../crit3dDate/debug/libcrit3dDate.a
    PRE_TARGETDEPS += ../mathFunctions/debug/libmathFunctions.a
}

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

unix:{
    PRE_TARGETDEPS += ../climate/release/libclimate.a
    PRE_TARGETDEPS += ../dbMeteoGrid/release/libdbMeteoGrid.a
    PRE_TARGETDEPS += ../dbMeteoPoints/release/libdbMeteoPoints.a
    PRE_TARGETDEPS += ../utilities/release/libutilities.a
    PRE_TARGETDEPS += ../solarRadiation/release/libsolarRadiation.a
    PRE_TARGETDEPS += ../interpolation/release/libinterpolation.a
    PRE_TARGETDEPS += ../meteo/release/libmeteo.a
    PRE_TARGETDEPS += ../gis/release/libgis.a
    PRE_TARGETDEPS += ../crit3dDate/release/libcrit3dDate.a
    PRE_TARGETDEPS += ../mathFunctions/release/libmathFunctions.a
}

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
    mainWindow.cpp \
    computationDialog.cpp \
    anomalyLayout.cpp \
    saveClimaLayout.cpp \
    shared/dialogWindows.cpp \
    shared/stationMarker.cpp \
    shared/rubberBand.cpp \
    shared/formInfo.cpp \
    shared/settingsDialog.cpp \
    shared/mapGraphicsRasterObject.cpp \
    shared/interpolationDialog.cpp \
    shared/interpolationCmd.cpp \
    shared/project.cpp \
    pragaProject.cpp \
    pragaSettingsDialog.cpp \
    pragaDialogs.cpp \
    climateFieldsDialog.cpp \
    keyboardFilter.cpp \
    shared/colorLegend.cpp


HEADERS  += mainWindow.h \
    saveClimaLayout.h \
    computationDialog.h \
    anomalyLayout.h \
    shared/dialogWindows.h \
    shared/stationMarker.h \
    shared/rubberBand.h \
    shared/formInfo.h \
    shared/settingsDialog.h \
    shared/mapGraphicsRasterObject.h \
    shared/interpolationDialog.h \
    shared/interpolationCmd.h \
    shared/project.h \
    pragaProject.h \
    pragaSettingsDialog.h \
    pragaDialogs.h \
    climateFieldsDialog.h \
    keyboardFilter.h \
    shared/colorLegend.h


FORMS    += mainWindow.ui \
        shared/formInfo.ui

