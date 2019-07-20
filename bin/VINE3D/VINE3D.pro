#----------------------------------------------------
#
#   VINE3D
#   Simulations of vineyard ecosystem
#   with 3D soil water balance
#
#   this project is part of CRITERIA3D distribution
#
#----------------------------------------------------

QT  += core gui widgets xml sql network

TARGET = VINE3D
TEMPLATE = app

INCLUDEPATH +=  ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/gis ../../lib/meteo \
                ../../lib/interpolation ../../lib/solarRadiation ../../lib/soil  \
                ../../lib/soilFluxes3D/header ../../lib/crop ../../lib/grapevine \
                ../../lib/utilities ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid \
                ../../lib/soilWidget ../../mapGraphics \
                ../PRAGA/shared ../CRITERIA3D/shared

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

    LIBS += -L../../lib/soilWidget/debug -lsoilWidget
    LIBS += -L../../lib/soil/debug -lsoil
    LIBS += -L../../lib/soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../../lib/grapevine/debug -lgrapevine
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

    LIBS += -L../../lib/soilWidget/release -lsoilWidget
    LIBS += -L../../lib/soil/release -lsoil
    LIBS += -L../../lib/soilFluxes3D/release -lsoilFluxes3D 
    LIBS += -L../../lib/grapevine/release -lgrapevine
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

SOURCES += \
    ../PRAGA/shared/interpolationCmd.cpp \
    ../PRAGA/shared/project.cpp \
    ../PRAGA/shared/formInfo.cpp \
    ../PRAGA/shared/dialogSettings.cpp \
    ../PRAGA/shared/dialogSelection.cpp \
    ../PRAGA/shared/shell.cpp \
    ../PRAGA/shared/stationMarker.cpp \
    ../PRAGA/shared/dialogInterpolation.cpp \
    ../CRITERIA3D/shared/soil3D.cpp \
    ../CRITERIA3D/shared/meteoMaps.cpp \
    ../PRAGA/shared/mapGraphicsRasterObject.cpp \
    ../PRAGA/shared/rubberBand.cpp \
    ../PRAGA/shared/colorLegend.cpp \
    atmosphere.cpp \
    dataHandler.cpp \
    disease.cpp \
    main.cpp \
    modelCore.cpp \
    plant.cpp \
    vine3DShell.cpp \
    waterBalance.cpp \
    vine3DProject.cpp \
    mainWindow.cpp \
    formPeriod.cpp

HEADERS  += \
    ../PRAGA/shared/interpolationCmd.h \
    ../PRAGA/shared/project.h \
    ../PRAGA/shared/formInfo.h \
    ../PRAGA/shared/dialogSettings.h \
    ../PRAGA/shared/dialogSelection.h \
    ../PRAGA/shared/shell.h \
    ../PRAGA/shared/stationMarker.h \
    ../PRAGA/shared/dialogInterpolation.h \
    ../CRITERIA3D/shared/soil3D.h \
    ../CRITERIA3D/shared/meteoMaps.h \
    ../PRAGA/shared/mapGraphicsRasterObject.h \
    ../PRAGA/shared/rubberBand.h \
    ../PRAGA/shared/colorLegend.h \
    atmosphere.h \
    dataHandler.h \
    disease.h \
    modelCore.h \
    plant.h \
    vine3DShell.h \
    waterBalance.h \
    vine3DProject.h \
    mainWindow.h \
    formPeriod.h

FORMS    += \
    ../PRAGA/shared/formInfo.ui \
    mainWindow.ui \
    formPeriod.ui
