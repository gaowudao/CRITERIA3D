#-------------------------------------------------
#
# CRITERIA3D
# gis library
#
#-------------------------------------------------

QT  -= core gui

TARGET = gis
TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

#CONFIG(debug, debug|release) {
#    TARGET = debug/gis
#} else {
#    TARGET = release/gis
#}

INCLUDEPATH += ../mathFunctions

SOURCES += gis.cpp \
    gisIO.cpp \
    color.cpp \
    map.cpp

HEADERS += gis.h \
    color.h \
    gisIO.h \
    map.h


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
