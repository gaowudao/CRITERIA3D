#----------------------------------------------------
#
#   Library specialMathFunctions
#   contains advanced math functions
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
        TARGET = debug/specialMathFunctions
    } else {
        TARGET = release/specialMathFunctions
    }
}
win32:{
    TARGET = specialMathFunctions
}

INCLUDEPATH += ../mathFunctions


SOURCES += \
    furtherMathFunctions.cpp \
    eispack.cpp

HEADERS += \
    furtherMathFunctions.h \
    eispack.h
