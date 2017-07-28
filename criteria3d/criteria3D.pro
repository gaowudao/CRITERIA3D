#-------------------------------------------------
#
# CRITERIA 3D
#
#-------------------------------------------------

QT  += core gui widgets xml sql

TARGET = Criteria3D
TEMPLATE = app

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo ../interpolation ../solarRadiation ../soil ../crop ../utilities
INCLUDEPATH += ../soilFluxes3D/header

LIBS += -L../crit3dDate/release -lcrit3dDate
LIBS += -L../mathFunctions/release -lmathFunctions
LIBS += -L../soilFluxes3D/release -lsoilFluxes3D
LIBS += -L../gis/release -lgis
LIBS += -L../meteo/release -lmeteo
LIBS += -L../crop/release -lcrop
LIBS += -L../interpolation/release -lInterpolation
LIBS += -L../solarRadiation/release -lsolarRadiation
LIBS += -L../soil/release -lsoil
LIBS += -L../utilities/release -lutilities


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
