#-------------------------------------------------
#
# CRITERIA3D distribution
# crit3dDate library
#
#-------------------------------------------------

QT       -= core gui

TARGET = crit3dDate
TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

#CONFIG(debug, debug|release) {
#    TARGET = debug/crit3dDate
#} else {
#    TARGET = release/crit3dDate
#}

INCLUDEPATH += ../mathFunctions

SOURCES += \
    crit3dDate.cpp \
    crit3dTime.cpp

HEADERS += \
    crit3dDate.h



