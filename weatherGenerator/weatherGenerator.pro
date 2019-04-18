#-------------------------------------------------------------------
#
# Library weatherGenerator
# this project is part of CRITERIA3D distribution
#
# Based on Richardson, C. W. and D. A. Wright,
# WGEN: A model for generating daily weather variables, USDA, 1984.
#
#-------------------------------------------------------------------


QT  += core xml
QT  -= gui

TARGET = weatherGenerator
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions

SOURCES += \
    timeUtility.cpp \
    parserXML.cpp \
    wgClimate.cpp \
    fileUtility.cpp \
    weatherGenerator.cpp

HEADERS += \
    timeUtility.h \
    parserXML.h \
    wgClimate.h \
    fileUtility.h \
    weatherGenerator.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
