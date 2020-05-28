#-----------------------------------------------------------
#
#   CropEditor
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------------

QT       += core gui widgets charts sql printsupport

TEMPLATE = app

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestCriteria1D
    } else {
        TARGET = release/TestCriteria1D
    }
}
macx:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestCriteria1D
    } else {
        TARGET = release/TestCriteria1D
    }
}
win32:{
    TARGET = CROP_EDITOR
}

INCLUDEPATH +=  ../../agrolib/crit3dDate ../../agrolib/mathFunctions ../../agrolib/gis  \
                ../../agrolib/meteo ../../agrolib/utilities ../../agrolib/soil ../../agrolib/crop \
                ../../agrolib/qcustomplot ../../agrolib/criteriaModel ../../agrolib/cropWidget

CONFIG(debug, debug|release) {
    LIBS += -L../../agrolib/cropWidget/debug -lcropWidget
    LIBS += -L../../agrolib/criteriaModel/debug -lcriteriaModel
    LIBS += -L../../agrolib/qcustomplot/debug -lqcustomplot
    LIBS += -L../../agrolib/crop/debug -lcrop
    LIBS += -L../../agrolib/soil/debug -lsoil
    LIBS += -L../../agrolib/utilities/debug -lutilities
    LIBS += -L../../agrolib/meteo/debug -lmeteo
    LIBS += -L../../agrolib/gis/debug -lgis
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate
} else {

    LIBS += -L../../agrolib/cropWidget/release -lcropWidget
    LIBS += -L../../agrolib/criteriaModel/release -lcriteriaModel
    LIBS += -L../../agrolib/qcustomplot/release -lqcustomplot
    LIBS += -L../../agrolib/crop/release -lcrop
    LIBS += -L../../agrolib/soil/release -lsoil
    LIBS += -L../../agrolib/utilities/release -lutilities
    LIBS += -L../../agrolib/meteo/release -lmeteo
    LIBS += -L../../agrolib/gis/release -lgis
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
}


SOURCES += \
        main.cpp



