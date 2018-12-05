#-----------------------------------------------------
#
# CRITERIA3D
# Soil Water Balance 3D
#
# this file is part of CRITERIA3D distribution
# CRITERIA3D has been developed under contract issued by ARPAE Emilia-Romagna
#
#-----------------------------------------------------

QT       += core gui network widgets sql xml 3dcore 3drender 3dextras

TARGET = CRITERIA3D
TEMPLATE = app

INCLUDEPATH +=  ../PRAGA/shared  \
                ../mapGraphics ../soilFluxes3D/header  \
                ../crit3dDate ../mathFunctions ../soil ../crop \
                ../meteo ../gis ../utilities  \
                ../interpolation ../solarRadiation  \
                ../dbMeteoPoints ../dbMeteoGrid


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
    LIBS += -L../dbMeteoGrid/debug -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/debug -ldbMeteoPoints
    LIBS += -L../utilities/debug -lutilities
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


SOURCES += main.cpp\
    mainwindow.cpp \
    ../PRAGA/shared/dialogWindows.cpp \
    ../PRAGA/shared/stationMarker.cpp \
    ../PRAGA/shared/settingsDialog.cpp \
    ../PRAGA/shared/interpolationDialog.cpp \
    ../PRAGA/shared/interpolationCmd.cpp \
    ../PRAGA/shared/rasterObject.cpp \
    ../PRAGA/shared/rubberBand.cpp \
    ../PRAGA/shared/colorlegend.cpp \
    ../PRAGA/shared/project.cpp \
    ../PRAGA/shared/formInfo.cpp \
    meteoMaps.cpp \
    waterBalance3D.cpp \
    crit3dProject.cpp \
    scene.cpp \
    viewer3D.cpp

HEADERS += mainwindow.h \
    ../PRAGA/shared/dialogWindows.h \
    ../PRAGA/shared/stationMarker.h \
    ../PRAGA/shared/settingsDialog.h \
    ../PRAGA/shared/interpolationDialog.h \
    ../PRAGA/shared/interpolationCmd.h  \
    ../PRAGA/shared/rasterObject.h \
    ../PRAGA/shared/rubberBand.h \
    ../PRAGA/shared/colorlegend.h \
    ../PRAGA/shared/project.h \
    ../PRAGA/shared/formInfo.h \
    meteoMaps.h \
    waterBalance3D.h \
    crit3dProject.h \
    scene.h \
    viewer3D.h


FORMS += mainwindow.ui \
    ../PRAGA/shared/formInfo.ui

