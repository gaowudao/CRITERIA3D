#-------------------------------------------------
#
# CRITERIA3D library
# common constants and math functions
#
#-------------------------------------------------

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
