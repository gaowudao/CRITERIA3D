#-------------------------------------------------
#
# CRITERIA3D library
# common constants and math functions
#
#-------------------------------------------------

QT       -= core gui

TARGET = mathFunctions
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    basicMath.cpp \
    furtherMathFunctions.cpp \
    statistics.cpp \
    gammaFunction.cpp \
    physics.cpp

HEADERS += \
    commonConstants.h \
    basicMath.h \
    furtherMathFunctions.h \
    statistics.h \
    gammaFunction.h \
    gammaFunction.h \
    physics.h
