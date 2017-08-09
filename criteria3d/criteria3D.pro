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

LIBS += -L../crit3dDate/debug -lcrit3dDate
LIBS += -L../mathFunctions/debug -lmathFunctions
LIBS += -L../soilFluxes3D/debug -lsoilFluxes3D
LIBS += -L../gis/debug -lgis
LIBS += -L../meteo/debug -lmeteo
LIBS += -L../crop/debug -lcrop
LIBS += -L../interpolation/debug -lInterpolation
LIBS += -L../solarRadiation/debug -lsolarRadiation
LIBS += -L../soil/debug -lsoil
LIBS += -L../utilities/debug -lutilities


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
