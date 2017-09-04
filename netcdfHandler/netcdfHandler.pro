#-------------------------------------------------
#
# Project created by QtCreator 2017-08-28T15:59:15
#
#-------------------------------------------------

QT       -= core gui

TARGET = netcdfHandler
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../mathFunctions ../gis ../crit3dDate
INCLUDEPATH += $$(NC4_INSTALL_DIR)/include

SOURCES += \
    netcdfHandler.cpp

HEADERS += \
    netcdfHandler.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
