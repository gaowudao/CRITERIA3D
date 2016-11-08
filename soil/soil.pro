#-------------------------------------------------
#
# CRITERIA3D
# soil library
#
#-------------------------------------------------

QT       -= core gui

TARGET = soil
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../mathFunctions

SOURCES += soil.cpp

HEADERS += soil.h


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
