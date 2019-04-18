#----------------------------------------------------
#
#   Library mathFunctions
#   contains common constants and defines
#   math, physics and statistic functions
#
#   this project is part of CRITERIA3D distribution
#
#----------------------------------------------------

QT       -= core gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/mathFunctions
    } else {
        TARGET = release/mathFunctions
    }
}
win32:{
    TARGET = mathFunctions
}

INCLUDEPATH += ../crit3dDate

SOURCES += \
    basicMath.cpp \
    furtherMathFunctions.cpp \
    statistics.cpp \
    gammaFunction.cpp \
    physics.cpp \
    eispack.cpp

HEADERS += \
    commonConstants.h \
    basicMath.h \
    furtherMathFunctions.h \
    statistics.h \
    gammaFunction.h \
    gammaFunction.h \
    physics.h \
    eispack.h
