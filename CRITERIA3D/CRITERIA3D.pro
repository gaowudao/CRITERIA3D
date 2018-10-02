#-----------------------------------------------------
#
# CRITERIA3D
# Soil Water Balance 3D
#
# this software is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += core gui widgets network sql xml

TARGET = CRITERIA3D
TEMPLATE = app

INCLUDEPATH +=  ../PRAGA/shared ../climate \
                ../MapGraphics ../soilFluxes3D/header \
                ../crit3dDate ../mathFunctions ../soil ../crop \
                ../meteo ../gis ../utilities \
                ../interpolation ../solarRadiation ../waterBalance3D \
                ../dbMeteoPoints ../dbMeteoGrid

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
    LIBS += -L../dbMeteoGrid/debug -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/debug -ldbMeteoPoints
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../waterBalance3D/debug -lwaterBalance3D
    LIBS += -L../solarRadiation/debug -lsolarRadiation
    LIBS += -L../interpolation/debug -linterpolation
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
    LIBS += -L../crop/debug -lcrop
    LIBS += -L../soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../dbMeteoGrid/release -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../waterBalance3D/release -lwaterBalance3D
    LIBS += -L../solarRadiation/release -lsolarRadiation
    LIBS += -L../interpolation/release -linterpolation
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
    LIBS += -L../crop/release -lcrop
    LIBS += -L../soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../soil/release -lsoil
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
}


#LIBS += -L$$(NC4_INSTALL_DIR)/lib -lnetcdf


SOURCES += main.cpp\
    mainwindow.cpp \
    dialogWindows.cpp \
    project.cpp \
    stationMarker.cpp \
    ../PRAGA/shared/settingsDialog.cpp \
    ../PRAGA/shared/interpolationDialog.cpp \
    ../PRAGA/shared/interpolationCmd.cpp \
    ../PRAGA/shared/formRunInfo.cpp \
    ../PRAGA/shared/rasterObject.cpp \
    ../PRAGA/shared/rubberBand.cpp \
    ../PRAGA/shared/colorlegend.cpp

HEADERS += mainwindow.h \
    dialogWindows.h \
    project.h \
    stationMarker.h \
    ../PRAGA/shared/settingsDialog.h \
    ../PRAGA/shared/interpolationDialog.h \
    ../PRAGA/shared/interpolationCmd.h  \
    ../PRAGA/shared/formRunInfo.h \
    ../PRAGA/shared/rasterObject.h \
    ../PRAGA/shared/rubberBand.h \
    ../PRAGA/shared/colorlegend.h

FORMS += mainwindow.ui \
    ../PRAGA/shared/formInfo.ui

