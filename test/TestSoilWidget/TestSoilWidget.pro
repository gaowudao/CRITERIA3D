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
macx:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestSoilWidget
    } else {
        TARGET = release/TestSoilWidget
    }
}
win32:{
    TARGET = TestSoilWidget
}

INCLUDEPATH +=  ../../agrolib/crit3dDate ../../agrolib/mathFunctions  \
                ../../agrolib/utilities ../../agrolib/soil ../../agrolib/soilWidget
unix:{
    INCLUDEPATH += /usr/include/qwt/
}
macx:{
    INCLUDEPATH += /usr/local/opt/qwt/lib/qwt.framework/Headers/
}

CONFIG(debug, debug|release) {
    LIBS += -L../../agrolib/soilWidget/debug -lsoilWidget
    LIBS += -L../../agrolib/soil/debug -lsoil
    LIBS += -L../../agrolib/utilities/debug -lutilities
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate
} else {

    LIBS += -L../../agrolib/soilWidget/release -lsoilWidget
    LIBS += -L../../agrolib/soil/release -lsoil
    LIBS += -L../../agrolib/utilities/release -lutilities
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
}


SOURCES += \
        main.cpp

win32:{
    include($$(QWT_ROOT)/features/qwt.prf)
}
unix:{
    include(/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qwt.prf)
}
macx:{
    include(/usr/local/opt/qwt/features/qwt.prf)
}

