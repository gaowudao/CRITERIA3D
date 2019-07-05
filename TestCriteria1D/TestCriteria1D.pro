#-----------------------------------------------------
#
#   TESTCRITERIA1D
#   Test for CriteriaModel library
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

INCLUDEPATH +=  ../crit3dDate ../mathFunctions ../soil ../crop  ../gis  \
                ../meteo ../utilities ../cropWidget ../soilWidget ../criteriaModel

CONFIG(debug, debug|release) {
    LIBS += -L../criteriaModel/debug -lcriteriaModel
    LIBS += -L../soilWidget/debug -lsoilWidget
    LIBS += -L../cropWidget/debug -lcropWidget
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
    LIBS += -L../crop/debug -lcrop
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions

} else {

    LIBS += -L../criteriaModel/release -lcriteriaModel
    LIBS += -L../soilWidget/release -lsoilWidget
    LIBS += -L../cropWidget/release -lcropWidget
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
    LIBS += -L../crop/release -lcrop
    LIBS += -L../soil/release -lsoil
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
}

