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

INCLUDEPATH +=  ../../mapGraphics \
                ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/meteo ../../lib/gis  \
                ../../lib/interpolation ../../lib/solarRadiation ../../lib/utilities  \
                ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid ../../lib/climate \
                ../../lib/netcdfHandler  ../../lib/shapeHandler  \
                ../../lib/graphics ../../lib/project

CONFIG += debug_and_release
QMAKE_CXXFLAGS += -std=c++11

DEFINES += NETCDF


LIBS += -L../../mapGraphics/release -lMapGraphics

win32:{
    LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf
}
unix:{
    LIBS += -lnetcdf
}
macx:{
    LIBS += -L/usr/local/lib/ -lnetcdf
}


CONFIG(debug, debug|release) {
    LIBS += -L../../lib/graphics/debug -lgraphics
    LIBS += -L../../lib/project/debug -lproject
    LIBS += -L../../lib/climate/debug -lclimate
    LIBS += -L../../lib/netcdfHandler/debug -lnetcdfHandler
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
    LIBS += -L../../lib/graphics/release -lgraphics
    LIBS += -L../../lib/project/release -lproject
    LIBS += -L../../lib/climate/release -lclimate
    LIBS += -L../../lib/netcdfHandler/release -lnetcdfHandler
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
    pragaProject.cpp \
    dialogMeteoComputation.cpp \
    dialogDownloadMeteoData.cpp \
    dialogClimateFields.cpp \
    dialogPragaSettings.cpp \
    dialogSeriesOnZones.cpp \
    dialogXMLComputation.cpp \
    dialogAnomaly.cpp \
    pragaShell.cpp


HEADERS  += mainWindow.h \
    dialogPragaProject.h \
    saveClimaLayout.h \
    pragaProject.h \
    dialogMeteoComputation.h \
    dialogDownloadMeteoData.h \
    dialogClimateFields.h \
    dialogPragaSettings.h \
    dialogSeriesOnZones.h \
    dialogXMLComputation.h \
    dialogAnomaly.h \
    pragaShell.h


FORMS    += mainWindow.ui


