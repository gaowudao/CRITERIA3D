#-----------------------------------------------------
#
#   CRITERIA3D
#   3D Soil Water Balance
#
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += core gui network widgets sql xml 3dcore 3drender 3dextras

TARGET = CRITERIA3D
TEMPLATE = app

INCLUDEPATH +=  ./shared ../PRAGA/shared  \
                ../mapGraphics ../soilFluxes3D/header  \
                ../crit3dDate ../mathFunctions ../crop \
                ../soil ../soilWidget \
                ../meteo ../gis ../utilities  \
                ../interpolation ../solarRadiation  \
                ../dbMeteoPoints ../dbMeteoGrid \

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
    LIBS += -L../soilWidget/debug -lsoilWidget
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
    LIBS += -L../soilWidget/release -lsoilWidget
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
    ../PRAGA/shared/dialogSelection.cpp \
    ../PRAGA/shared/stationMarker.cpp \
    ../PRAGA/shared/dialogSettings.cpp \
    ../PRAGA/shared/dialogInterpolation.cpp \
    ../PRAGA/shared/interpolationCmd.cpp \
    ../PRAGA/shared/mapGraphicsRasterObject.cpp \
    ../PRAGA/shared/rubberBand.cpp \
    ../PRAGA/shared/colorLegend.cpp \
    ../PRAGA/shared/project.cpp \
    ../PRAGA/shared/formInfo.cpp \
    waterBalance3D.cpp \
    crit3dProject.cpp \
    viewer3d.cpp \
    shared/soil3D.cpp \
    shared/meteoMaps.cpp


HEADERS += mainwindow.h \
    ../PRAGA/shared/dialogSelection.h \
    ../PRAGA/shared/stationMarker.h \
    ../PRAGA/shared/dialogSettings.h \
    ../PRAGA/shared/dialogInterpolation.h \
    ../PRAGA/shared/interpolationCmd.h  \
    ../PRAGA/shared/mapGraphicsRasterObject.h \
    ../PRAGA/shared/rubberBand.h \
    ../PRAGA/shared/colorLegend.h \
    ../PRAGA/shared/project.h \
    ../PRAGA/shared/formInfo.h \
    waterBalance3D.h \
    crit3dProject.h \
    viewer3d.h \
    shared/soil3D.h \
    shared/meteoMaps.h


FORMS += mainwindow.ui \
    ../PRAGA/shared/formInfo.ui

