#-------------------------------------------------
#
# CRITERIA 3D
#
#-------------------------------------------------

QT  += core gui widgets xml sql

TARGET = Criteria3D
TEMPLATE = app

INCLUDEPATH += ../crit3dDate
LIBS += -L../crit3dDate/release -lcrit3dDate

INCLUDEPATH += ../gis
LIBS += -L../gis/release -lgis

INCLUDEPATH += ../meteo
LIBS += -L../meteo/release -lmeteo

INCLUDEPATH += ../crop
LIBS += -L../crop/release -lcrop

INCLUDEPATH += ../interpolation
LIBS += -L../interpolation/release -lInterpolation

INCLUDEPATH += ../mathFunctions
LIBS += -L../mathFunctions/release -lmathFunctions

INCLUDEPATH += ../solarRadiation
LIBS += -L../solarRadiation/release -lsolarRadiation

INCLUDEPATH += ../soil
LIBS += -L../soil/release -lsoil

INCLUDEPATH += ../soilFluxes3D/header
LIBS += -L../soilFluxes3D/release -lsoilFluxes3D

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
