#-------------------------------------------------
#
# VINE 3D Project
# simulations of vineyard ecosystem
# with 3D soil-water balance
#
#-------------------------------------------------

QT  += core gui widgets xml sql network

TARGET = VINE3D
TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo ../interpolation ../solarRadiation \
                ../soil ../utilities ../soilFluxes3D/header ../dbMeteoPoints ../dbMeteoGrid \
                ../grapevine ../powderyMildew ../downyMildew \
                ../MapGraphics ../PRAGA/shared ../climate

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
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../gis/debug -lgis
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../interpolation/debug -lInterpolation
    LIBS += -L../solarRadiation/debug -lsolarRadiation
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../dbMeteoGrid/debug -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/debug -ldbMeteoPoints
    LIBS += -L../grapevine/debug -lgrapevine
    LIBS += -L../powderyMildew/debug -lpowderyMildew
    LIBS += -L../downyMildew/debug -ldownyMildew
} else {
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
    LIBS += -L../gis/release -lgis
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../interpolation/release -lInterpolation
    LIBS += -L../solarRadiation/release -lsolarRadiation
    LIBS += -L../soil/release -lsoil
    LIBS += -L../soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../dbMeteoGrid/release -ldbMeteoGrid
    LIBS += -L../dbMeteoPoints/release -ldbMeteoPoints
    LIBS += -L../grapevine/release -lgrapevine
    LIBS += -L../powderyMildew/release -lpowderyMildew
    LIBS += -L../downyMildew/release -ldownyMildew
}

SOURCES += \
    ../PRAGA/shared/interpolationCmd.cpp \
    ../PRAGA/shared/project.cpp \
    ../PRAGA/shared/formInfo.cpp \
    ../PRAGA/shared/settingsDialog.cpp \
    ../PRAGA/shared/rubberBand.cpp \
    ../PRAGA/shared/stationMarker.cpp \
    ../PRAGA/shared/colorlegend.cpp \
    ../PRAGA/shared/rasterObject.cpp \
    ../PRAGA/shared/dialogWindows.cpp \
    ../PRAGA/shared/interpolationDialog.cpp \
    atmosphere.cpp \
    dataHandler.cpp \
    disease.cpp \
    main.cpp \
    meteoMaps.cpp \
    modelCore.cpp \
    parserXML.cpp \
    plant.cpp \
    waterBalance.cpp \
    vine3DProject.cpp \
    mainWindow.cpp \
    formPeriod.cpp

HEADERS  += \
    ../PRAGA/shared/interpolationCmd.h \
    ../PRAGA/shared/project.h \
    ../PRAGA/shared/formInfo.h \
    ../PRAGA/shared/settingsDialog.h \
    ../PRAGA/shared/rubberBand.h \
    ../PRAGA/shared/stationMarker.h \
    ../PRAGA/shared/colorlegend.h \
    ../PRAGA/shared/rasterObject.h \
    ../PRAGA/shared/dialogWindows.h \
    ../PRAGA/shared/interpolationDialog.h \
    atmosphere.h \
    dataHandler.h \
    disease.h \
    meteoMaps.h \
    modelCore.h \
    parserXML.h \
    plant.h \
    waterBalance.h \
    vine3DProject.h \
    mainWindow.h \
    formPeriod.h

FORMS    += \
    ../PRAGA/shared/formInfo.ui \
    mainWindow.ui \
    formPeriod.ui
