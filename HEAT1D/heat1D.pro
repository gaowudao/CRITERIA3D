#-------------------------------------------------
#
#   HEAT1D
#   test for soilFluxex3D library in a 1D domain
#   This project is part of CRITERIA3D distribution
#
#-------------------------------------------------

QT       += widgets core

TARGET = HEAT1D
TEMPLATE = app

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    LIBS += -L../soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../gis/debug -lgis
} else {
    LIBS += -L../soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../mathFunctions/release -lmathFunctions
    LIBS += -L../gis/release -lgis
}

INCLUDEPATH += ../mathFunctions
INCLUDEPATH += ../soilFluxes3D/header
INCLUDEPATH += ../gis

SOURCES += main.cpp\
    mainwindow.cpp \
    heat1D.cpp \
    graphs.cpp

HEADERS  += heat1D.h \
    mainwindow.h \
    graphs.h

FORMS    += \
    mainwindow.ui

include($$(QWT_ROOT)\features\qwt.prf)
