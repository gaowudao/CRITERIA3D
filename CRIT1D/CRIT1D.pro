
QT -= GUI
QT += core sql

TARGET = CRITERIA1D

CONFIG += console

TEMPLATE = app

SOURCES += main.cpp \
    project.cpp \

HEADERS += \
    project.h \

INCLUDEPATH += ../crit3dDate ../mathFunctions ../utilities ../crop ../gis ../meteo ../soil ../Criteria1D

CONFIG(debug, debug|release) {
    LIBS += -L../Criteria1D/debug -lCriteria1D
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
    LIBS += -L../crop/debug -lcrop
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../Criteria1D/release -lCriteria1D
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
    LIBS += -L../crop/release -lcrop
    LIBS += -L../soil/release -lsoil
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
}

