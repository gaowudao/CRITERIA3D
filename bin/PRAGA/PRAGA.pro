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
                ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid ../../lib/climate ../../lib/netcdfHandler

CONFIG += debug_and_release

DEFINES += NETCDF


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

    LIBS += -L../../lib/climate/debug -lclimate
    LIBS += -L../../lib/netcdfHandler/debug -lnetcdfHandler
    win32:{
        LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf
    }
    unix:{
        LIBS += -lnetcdf
    }
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

    LIBS += -L../../lib/climate/release -lclimate
    LIBS += -L../../lib/netcdfHandler/release -lnetcdfHandler
    win32:{
        LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf
    }
    unix:{
        LIBS += -lnetcdf
    }
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


SOURCES += main.cpp\
    dialogPragaProject.cpp \
    mainWindow.cpp \
    saveClimaLayout.cpp \
    shared/dialogProject.cpp \
    shared/meteoMaps.cpp \
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
    pragaShell.cpp \
    shared/dialogRadiation.cpp \
    shared/formPeriod.cpp


HEADERS  += mainWindow.h \
    dialogPragaProject.h \
    saveClimaLayout.h \
    shared/dialogProject.h \
    shared/meteoMaps.h \
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
    pragaShell.h \
    shared/dialogRadiation.h \
    shared/formPeriod.h


FORMS    += mainWindow.ui \
        shared/formInfo.ui \
    shared/formPeriod.ui

