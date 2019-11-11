#-----------------------------------------------------
#
#   TESTCRITERIA1D
#   Test for CriteriaModel library
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------


QT -= gui
QT += core sql

CONFIG += console

TEMPLATE = app

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestCriteria1D
    } else {
        TARGET = release/TestCriteria1D
    }
}
win32:{
    TARGET = TestCriteria1D
}

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

INCLUDEPATH +=  ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/soil \
                ../../lib/crop ../../lib/gis ../../lib/meteo ../../lib/utilities \
                ../../lib/cropWidget ../../lib/criteriaModel

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/criteriaModel/debug -lcriteriaModel
    LIBS += -L../../lib/cropWidget/debug -lcropWidget
    LIBS += -L../../lib/crop/debug -lcrop
    LIBS += -L../../lib/soil/debug -lsoil
    LIBS += -L../../lib/utilities/debug -lutilities
    LIBS += -L../../lib/meteo/debug -lmeteo
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions

} else {

    LIBS += -L../../lib/criteriaModel/release -lcriteriaModel
    LIBS += -L../../lib/cropWidget/release -lcropWidget
    LIBS += -L../../lib/crop/release -lcrop
    LIBS += -L../../lib/soil/release -lsoil
    LIBS += -L../../lib/utilities/release -lutilities
    LIBS += -L../../lib/meteo/release -lmeteo
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
}

