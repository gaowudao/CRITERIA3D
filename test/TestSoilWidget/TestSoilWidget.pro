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
    TARGET = TestSoilWidget
}
win32:{
    TARGET = TestSoilWidget
}

INCLUDEPATH +=  ../../lib/crit3dDate ../../lib/mathFunctions  \
                ../../lib/utilities ../../lib/soil ../../lib/soilWidget
unix:{
    INCLUDEPATH += /usr/include/qwt/
}

macx:{
    INCLUDEPATH += /usr/local/opt/qwt/lib/qwt.framework/Headers/
}

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/soilWidget/debug -lsoilWidget
    LIBS += -L../../lib/utilities/debug -lutilities
    LIBS += -L../../lib/soil/debug -lsoil
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
} else {

    LIBS += -L../../lib/soilWidget/release -lsoilWidget
    LIBS += -L../../lib/utilities/release -lutilities
    LIBS += -L../../lib/soil/release -lsoil
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
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

