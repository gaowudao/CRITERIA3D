
QT -= GUI
QT += core sql network

TARGET = MOSES_WB

CONFIG += esri_runtime_qt10_2_6
CONFIG += console

TEMPLATE = app

SOURCES += main.cpp \
    project.cpp \
    queryArcgis.cpp

HEADERS += \
    project.h \
    queryArcgis.h

INCLUDEPATH += ../crit3dDate ../mathFunctions ../utilities ../crop ../gis ../meteo ../soil ../Criteria1D

LIBS += -L../Criteria1D/release -lCriteria1D
LIBS += -L../utilities/release -lutilities
LIBS += -L../meteo/release -lmeteo
LIBS += -L../gis/release -lgis
LIBS += -L../crop/release -lcrop
LIBS += -L../soil/release -lsoil
LIBS += -L../crit3dDate/release -lcrit3dDate
LIBS += -L../mathFunctions/release -lmathFunctions

