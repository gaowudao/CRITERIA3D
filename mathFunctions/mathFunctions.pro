#-------------------------------------------------
#
# CRITERIA3D distribution
# Common constants and math functions
#
#-------------------------------------------------

QT       -= core gui

TARGET = mathFunctions
TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

#CONFIG(debug, debug|release) {
#    TARGET = debug/mathFunctions
#} else {
#    TARGET = release/mathFunctions
#}

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
