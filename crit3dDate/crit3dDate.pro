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

SOURCES += \
    crit3dDate.cpp \
    crit3dTime.cpp

HEADERS += \
    crit3dDate.h

INCLUDEPATH += ../mathFunctions

