#-------------------------------------------------
#
# CRITERIA 3D
#
#-------------------------------------------------

QT  += core gui widgets xml sql

TARGET = Criteria3D
TEMPLATE = app


INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo ../quality ../interpolation \
                ../solarRadiation ../soil ../crop ../utilities ../soilFluxes3D/header

CONFIG += debug_and_release


CONFIG(debug, debug|release) {
    LIBS += -L../quality/debug -lquality
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../solarRadiation/debug -lsolarRadiation
    LIBS += -L../interpolation/debug -linterpolation
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../crop/debug -lcrop
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../quality/release -lquality
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../solarRadiation/release -lsolarRadiation
    LIBS += -L../interpolation/release -linterpolation
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
    LIBS += -L../soil/release -lsoil
    LIBS += -L../crop/release -lcrop
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../mathFunctions/release -lmathFunctions
}


SOURCES += main.cpp \
    mainwindow.cpp \
    mapWidget.cpp \
    formDate.cpp \
    formSingleValue.cpp \
    dataHandler.cpp \
    project.cpp \
    parserXML.cpp \
    modelCore.cpp \
    atmosphere.cpp \
    plant.cpp \
    waterBalance.cpp \
    meteoMaps.cpp \

HEADERS  += mainwindow.h \
    mapWidget.h \
    formSingleValue.h \
    formDate.h \
    dataHandler.h \
    project.h \
    parserXML.h \
    modelCore.h \
    atmosphere.h \
    plant.h \
    waterBalance.h \
    meteoMaps.h \


FORMS    += mainwindow.ui \
    formSingleValue.ui \
    formDate.ui


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
