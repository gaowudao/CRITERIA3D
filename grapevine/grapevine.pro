#-------------------------------------------------
#
# Project created by QtCreator 2015-04-30T11:28:07
#
#-------------------------------------------------

QT       -= gui

TARGET = grapevine
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += C:/GitHub/CRITERIA3D/crit3dDate
INCLUDEPATH += C:/GitHub/CRITERIA3D/mathFunctions
INCLUDEPATH += C:/GitHub/CRITERIA3D/soil

SOURCES += grapevine.cpp

HEADERS += grapevine.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
