#-------------------------------------------------
#
# CRITERIA3D
# crop library
#
#-------------------------------------------------

QT      -= core gui

TARGET = crop
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../mathFunctions ../crit3dDate ../soil

SOURCES += crop.cpp \
    root.cpp \
    development.cpp \
    biomass.cpp \
    grapevine.cpp

HEADERS += crop.h \
    biomass.h \
    root.h \
    development.h \
    grapevine.h


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
