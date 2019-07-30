#-----------------------------------------------------
#
#   PRAGA
#   PRogram for AGrometeorological Analysis
#
#   this project is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += core gui widgets network sql xml

TARGET = PRAGA
TEMPLATE = app

INCLUDEPATH +=  ./shared ../../mapGraphics \
                ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/meteo ../../lib/gis \
                ../../lib/interpolation ../../lib/solarRadiation ../../lib/utilities \
                ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid ../../lib/climate

CONFIG += debug_and_release

unix:{
    LIBS += -L../../mapGraphics/release -lMapGraphics
}
win32:{
    CONFIG(debug, debug|release) {
         LIBS += -L../../mapGraphics/debug -lMapGraphics
    } else {
        LIBS += -L../../mapGraphics/release -lMapGraphics
    }
}

CONFIG(debug, debug|release) {

unix:{
    PRE_TARGETDEPS += ../../lib/climate/debug/libclimate.a
    PRE_TARGETDEPS += ../../lib/dbMeteoGrid/debug/libdbMeteoGrid.a
    PRE_TARGETDEPS += ../../lib/dbMeteoPoints/debug/libdbMeteoPoints.a
    PRE_TARGETDEPS += ../../lib/utilities/debug/libutilities.a
    PRE_TARGETDEPS += ../../lib/solarRadiation/debug/libsolarRadiation.a
    PRE_TARGETDEPS += ../../lib/interpolation/debug/libinterpolation.a
    PRE_TARGETDEPS += ../../lib/meteo/debug/libmeteo.a
    PRE_TARGETDEPS += ../../lib/gis/debug/libgis.a
    PRE_TARGETDEPS += ../../lib/crit3dDate/debug/libcrit3dDate.a
    PRE_TARGETDEPS += ../../lib/mathFunctions/debug/libmathFunctions.a
}

    LIBS += -L../../lib/climate/debug -lclimate
    LIBS += -L../../lib/dbMeteoGrid/debug -ldbMeteoGrid
    LIBS += -L../../lib/dbMeteoPoints/debug -ldbMeteoPoints
    LIBS += -L../../lib/utilities/debug -lutilities
    LIBS += -L../../lib/solarRadiation/debug -lsolarRadiation
    LIBS += -L../../lib/interpolation/debug -linterpolation
    LIBS += -L../../lib/meteo/debug -lmeteo
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions

} else {

unix:{
    PRE_TARGETDEPS += ../../lib/climate/release/libclimate.a
    PRE_TARGETDEPS += ../../lib/dbMeteoGrid/release/libdbMeteoGrid.a
    PRE_TARGETDEPS += ../../lib/dbMeteoPoints/release/libdbMeteoPoints.a
    PRE_TARGETDEPS += ../../lib/utilities/release/libutilities.a
    PRE_TARGETDEPS += ../../lib/solarRadiation/release/libsolarRadiation.a
    PRE_TARGETDEPS += ../../lib/interpolation/release/libinterpolation.a
    PRE_TARGETDEPS += ../../lib/meteo/release/libmeteo.a
    PRE_TARGETDEPS += ../../lib/gis/release/libgis.a
    PRE_TARGETDEPS += ../../lib/crit3dDate/release/libcrit3dDate.a
    PRE_TARGETDEPS += ../../lib/mathFunctions/release/libmathFunctions.a
}

    LIBS += -L../../lib/climate/release -lclimate
    LIBS += -L../../lib/dbMeteoGrid/release -ldbMeteoGrid
    LIBS += -L../../lib/dbMeteoPoints/release -ldbMeteoPoints
    LIBS += -L../../lib/utilities/release -lutilities
    LIBS += -L../../lib/solarRadiation/release -lsolarRadiation
    LIBS += -L../../lib/interpolation/release -linterpolation
    LIBS += -L../../lib/meteo/release -lmeteo
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
}


#ifdef NETCDF
#LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf


SOURCES += main.cpp\
    mainWindow.cpp \
    saveClimaLayout.cpp \
    shared/dialogProject.cpp \
    shared/stationMarker.cpp \
    shared/rubberBand.cpp \
    shared/formInfo.cpp \
    shared/mapGraphicsRasterObject.cpp \
    shared/interpolationCmd.cpp \
    shared/project.cpp \
    pragaProject.cpp \
    shared/colorLegend.cpp \
    dialogMeteoComputation.cpp \
    dialogDownloadMeteoData.cpp \
    shared/dialogSelection.cpp \
    shared/dialogInterpolation.cpp \
    shared/dialogSettings.cpp \
    dialogClimateFields.cpp \
    dialogPragaSettings.cpp \
    dialogSeriesOnZones.cpp \
    dialogAnomaly.cpp \
    shared/shell.cpp \
    pragaShell.cpp


HEADERS  += mainWindow.h \
    saveClimaLayout.h \
    shared/dialogProject.h \
    shared/stationMarker.h \
    shared/rubberBand.h \
    shared/formInfo.h \
    shared/mapGraphicsRasterObject.h \
    shared/interpolationCmd.h \
    shared/project.h \
    pragaProject.h \
    shared/colorLegend.h \
    dialogMeteoComputation.h \
    dialogDownloadMeteoData.h \
    shared/dialogSelection.h \
    shared/dialogInterpolation.h \
    shared/dialogSettings.h \
    dialogClimateFields.h \
    dialogPragaSettings.h \
    dialogSeriesOnZones.h \
    dialogAnomaly.h \
    shared/shell.h \
    pragaShell.h


FORMS    += mainWindow.ui \
        shared/formInfo.ui

