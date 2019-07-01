#-----------------------------------------------------
#
#   TESTCRITERIA1D
#   Test for Criteria1D library
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------


QT -= gui
QT += core sql

TARGET = CRITERIA1D

CONFIG += console

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += main.cpp \
    project.cpp \

HEADERS += \
    project.h \

INCLUDEPATH +=  ../crit3dDate ../mathFunctions ../crop ../gis   \
                ../meteo ../soil ../utilities ../criteria1D

CONFIG(debug, debug|release) {
    LIBS += -L../criteria1D/debug -lcriteria1D
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
    LIBS += -L../crop/debug -lcrop
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../criteria1D/release -lcriteria1D
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
    LIBS += -L../crop/release -lcrop
    LIBS += -L../soil/release -lsoil
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
}

