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
                ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/meteo ../../lib/gis  \
                ../../lib/interpolation ../../lib/solarRadiation ../../lib/utilities  \
                ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid ../../lib/climate ../../lib/netcdfHandler  \
                ../../lib/project

CONFIG += debug_and_release
QMAKE_CXXFLAGS += -std=c++11

DEFINES += NETCDF

win32:{
    CONFIG(debug, debug|release) {
         LIBS += -L../../mapGraphics/debug -lMapGraphics
    } else {
        LIBS += -L../../mapGraphics/release -lMapGraphics
    }
}
else:{
    LIBS += -L../../mapGraphics/release -lMapGraphics
}

CONFIG(debug, debug|release) {

    LIBS += -L../../lib/climate/debug -lclimate
    LIBS += -L../../lib/netcdfHandler/debug -lnetcdfHandler
    win32:{
        LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf
    }
    else:{
        LIBS += -lnetcdf
    }

    LIBS += -L../../lib/project/debug -lproject
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
    macx:{
        LIBS += -L/usr/local/lib/ -lnetcdf
    }

    LIBS += -L../../lib/project/release -lproject
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
    shared/stationMarker.cpp \
    shared/rubberBand.cpp \
    shared/mapGraphicsRasterObject.cpp \
    shared/colorLegend.cpp \
    dialogPragaProject.cpp \
    mainWindow.cpp \
    saveClimaLayout.cpp \
    pragaProject.cpp \
    shared/colorLegend.cpp \
    dialogMeteoComputation.cpp \
    dialogDownloadMeteoData.cpp \
    dialogClimateFields.cpp \
    dialogPragaSettings.cpp \
    dialogSeriesOnZones.cpp \
    dialogAnomaly.cpp \
    pragaShell.cpp \
    dialogXMLComputation.cpp


HEADERS  += mainWindow.h \
    shared/stationMarker.h \
    shared/rubberBand.h \
    shared/mapGraphicsRasterObject.h \
    shared/colorLegend.h \
    dialogPragaProject.h \
    saveClimaLayout.h \
    pragaProject.h \
    dialogMeteoComputation.h \
    dialogDownloadMeteoData.h \
    dialogClimateFields.h \
    dialogPragaSettings.h \
    dialogSeriesOnZones.h \
    dialogAnomaly.h \
    pragaShell.h \
    dialogXMLComputation.h


FORMS    += mainWindow.ui


