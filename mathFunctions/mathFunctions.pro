#-------------------------------------------------
#
# CRITERIA 3D
# common constants and math functions
#
#-------------------------------------------------

QT       -= core gui

TARGET = mathFunctions
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    basicMath.cpp \
    gammafunction.cpp \
    furtherMathFunctions.cpp

HEADERS += \
    commonConstants.h \
    basicMath.h \
    gammafunction.h \
    furtherMathFunctions.h
