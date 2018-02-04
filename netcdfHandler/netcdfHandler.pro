#-------------------------------------------------
#
# CRITERIA3D distribution
# netcdfHandler library
#
#-------------------------------------------------

QT       -= core gui

TARGET = netcdfHandler
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../mathFunctions ../crit3dDate ../gis
INCLUDEPATH += $$(NC4_INSTALL_DIR)/include

SOURCES += \
    netcdfHandler.cpp

HEADERS += \
    netcdfHandler.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
