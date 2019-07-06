#-----------------------------------------------------
#
#   HEAT1D
#   Test for soilFluxex3D library in a 1D domain
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += widgets core

TARGET = HEAT1D
TEMPLATE = app

INCLUDEPATH += ../../lib/mathFunctions
INCLUDEPATH += ../../lib/soilFluxes3D/header
INCLUDEPATH += ../../lib/gis

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../gis/release -lgis
    LIBS += -L../soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../mathFunctions/release -lmathFunctions
}


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
