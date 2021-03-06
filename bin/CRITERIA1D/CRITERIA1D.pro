#-----------------------------------------------------
#
#   CRITERIA-1D
#
#   Water balance 1D
#
#   This project is part of CRITERIA-3D distribution
#
#-----------------------------------------------------


QT -= gui
QT += core sql xml

CONFIG += console
TEMPLATE = app

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/CRITERIA1D
    } else {
        TARGET = release/CRITERIA1D
    }
}
win32:{
    TARGET = CRITERIA1D
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += \
    main.cpp \
    criteria1DProject.cpp

HEADERS += \
    criteria1DProject.h

INCLUDEPATH +=  ../../agrolib/crit3dDate ../../agrolib/mathFunctions ../../agrolib/gis ../../agrolib/meteo \
                ../../agrolib/soil ../../agrolib/crop ../../agrolib/utilities ../../agrolib/criteriaModel

CONFIG(debug, debug|release) {
    LIBS += -L../../agrolib/criteriaModel/debug -lcriteriaModel
    LIBS += -L../../agrolib/crop/debug -lcrop
    LIBS += -L../../agrolib/soil/debug -lsoil
    LIBS += -L../../agrolib/utilities/debug -lutilities
    LIBS += -L../../agrolib/meteo/debug -lmeteo
    LIBS += -L../../agrolib/gis/debug -lgis
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions

} else {

    LIBS += -L../../agrolib/criteriaModel/release -lcriteriaModel
    LIBS += -L../../agrolib/crop/release -lcrop
    LIBS += -L../../agrolib/soil/release -lsoil
    LIBS += -L../../agrolib/utilities/release -lutilities
    LIBS += -L../../agrolib/meteo/release -lmeteo
    LIBS += -L../../agrolib/gis/release -lgis
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
}

