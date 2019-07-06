#-----------------------------------------------------------
#
#   TestSoilWidget
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------------

QT       += core gui widgets sql

TEMPLATE = app

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestSoilWidget
    } else {
        TARGET = release/TestSoilWidget
    }
}
win32:{
    TARGET = TestSoilWidget
}

INCLUDEPATH += ../../lib/mathFunctions ../../lib/crit3dDate ../../lib/utilities ../../lib/soil ../../lib/soilWidget

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/soilWidget/debug -lsoilWidget
    LIBS += -L../../lib/soil/debug -lsoil
    LIBS += -L../../lib/utilities/debug -lutilities
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
} else {

    LIBS += -L../../lib/soilWidget/release -lsoilWidget
    LIBS += -L../../lib/soil/release -lsoil
    LIBS += -L../../lib/utilities/release -lutilities
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
}


SOURCES += \
        main.cpp


