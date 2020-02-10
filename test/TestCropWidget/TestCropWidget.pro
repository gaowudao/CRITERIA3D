#-----------------------------------------------------------
#
#   TestCropWidget
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------------

QT       += core gui widgets sql

TEMPLATE = app

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestCropWidget
    } else {
        TARGET = release/TestCropWidget
    }
}
macx:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TestCropWidget
    } else {
        TARGET = release/TestCropWidget
    }
}
win32:{
    TARGET = TestCropWidget
}

INCLUDEPATH +=  ../../agrolib/crit3dDate ../../agrolib/mathFunctions  \
                ../../agrolib/utilities ../../agrolib/soil ../../agrolib/crop ../../agrolib/cropWidget

CONFIG(debug, debug|release) {
    LIBS += -L../../agrolib/cropWidget/debug -lcropWidget
    LIBS += -L../../agrolib/crop/debug -lcrop
    LIBS += -L../../agrolib/utilities/debug -lutilities
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate
} else {

    LIBS += -L../../agrolib/cropWidget/release -lcropWidget
    LIBS += -L../../agrolib/crop/release -lcrop
    LIBS += -L../../agrolib/utilities/release -lutilities
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
}


SOURCES += \
        main.cpp



