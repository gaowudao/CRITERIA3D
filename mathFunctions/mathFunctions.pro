#----------------------------------------------------
#
#   Library mathFunctions
#   contains common constants and defines
#   basic math, physics and statistic functions
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

HEADERS += \
    commonConstants.h \
    basicMath.h \
    statistics.h \
    physics.h

SOURCES += \
    basicMath.cpp \
    statistics.cpp \
    physics.cpp

