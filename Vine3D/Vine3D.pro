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

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo
INCLUDEPATH += ../quality ../interpolation ../solarRadiation ../soil
INCLUDEPATH += ../utilities ../soilFluxes3D/header
INCLUDEPATH += ../dbMeteoPoints ../dbMeteoGrid
INCLUDEPATH += ../grapevine ../powderyMildew ../downyMildew
INCLUDEPATH += ../PRAGA/shared

CONFIG(debug, debug|release) {
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../gis/debug -lgis
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../quality/debug -lquality
    LIBS += -L../Interpolation/debug -lInterpolation
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
    LIBS += -L../quality/release -lquality
    LIBS += -L../Interpolation/release -lInterpolation
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
    ../PRAGA/shared/interpolationCmd.cpp \
    ../PRAGA/shared/project.cpp \
    ../PRAGA/shared/formInfo.cpp

HEADERS  += \
    atmosphere.h \
    dataHandler.h \
    disease.h \
    meteoMaps.h \
    modelCore.h \
    parserXML.h \
    plant.h \
    waterBalance.h \
    vine3DProject.h \
    ../PRAGA/shared/interpolationCmd.h \
    ../PRAGA/shared/project.h \
    ../PRAGA/shared/formInfo.h

FORMS    += \
    ../PRAGA/shared/formInfo.ui
