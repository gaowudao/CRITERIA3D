#-----------------------------------------------------
#
#   CRITERIA3D
#   3D soil water balance
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += core gui network widgets sql xml 3dcore 3drender 3dextras

TARGET = CRITERIA3D
TEMPLATE = app


INCLUDEPATH +=  ./shared ../PRAGA/shared  \
                ../../mapGraphics \
                ../../lib/soilFluxes3D/header  \
                ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/crop \
                ../../lib/soil ../../lib/soilWidget \
                ../../lib/meteo ../../lib/gis ../../lib/utilities  \
                ../../lib/interpolation ../../lib/solarRadiation  \
                ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid \

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
    LIBS += -L../../lib/dbMeteoGrid/debug -ldbMeteoGrid
    LIBS += -L../../lib/dbMeteoPoints/debug -ldbMeteoPoints
    LIBS += -L../../lib/soilWidget/debug -lsoilWidget
    LIBS += -L../../lib/utilities/debug -lutilities
    LIBS += -L../../lib/solarRadiation/debug -lsolarRadiation
    LIBS += -L../../lib/interpolation/debug -linterpolation
    LIBS += -L../../lib/meteo/debug -lmeteo
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/crop/debug -lcrop
    LIBS += -L../../lib/soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../../lib/soil/debug -lsoil
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions

} else {

    LIBS += -L../../lib/dbMeteoGrid/release -ldbMeteoGrid
    LIBS += -L../../lib/dbMeteoPoints/release -ldbMeteoPoints
    LIBS += -L../../lib/soilWidget/release -lsoilWidget
    LIBS += -L../../lib/utilities/release -lutilities
    LIBS += -L../../lib/solarRadiation/release -lsolarRadiation
    LIBS += -L../../lib/interpolation/release -linterpolation
    LIBS += -L../../lib/meteo/release -lmeteo
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/crop/release -lcrop
    LIBS += -L../../lib/soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../../lib/soil/release -lsoil
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
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
    shared/meteoMaps.cpp \
    shared/project3D.cpp \
    shared/sharedMainWindow.cpp \
    waterBalance3D.cpp \
    viewer3d.cpp \
    crit3dProject.cpp


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
    shared/meteoMaps.h \
    shared/project3D.h \
    shared/sharedMainWindow.h \
    waterBalance3D.h \
    viewer3d.h \
    crit3dProject.h


FORMS += mainwindow.ui \
    ../PRAGA/shared/formInfo.ui

