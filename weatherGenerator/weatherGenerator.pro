#-------------------------------------------------
#
# Project created by QtCreator 2017-07-24T17:56:50
#
#-------------------------------------------------

QT  += core xml
QT  -= gui

TARGET = weatherGenerator
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions

SOURCES += weathergenerator.cpp \
    timeUtility.cpp \
    parserXML.cpp \
    wgClimate.cpp

HEADERS += weathergenerator.h \
    timeUtility.h \
    parserXML.h \
    wgClimate.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
