#----------------------------------------------------
#
# CRITERIA3D distribution
# Library of math, physics and statistic functions
# Contains common constants and defines
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
