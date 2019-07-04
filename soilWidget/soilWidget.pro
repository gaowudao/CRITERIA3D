#----------------------------------------------------
#
#   Soil Widget
#   This project is part of CRITERIA-3D distribution
#
#----------------------------------------------------

QT  += widgets sql

TARGET = soilWidget
TEMPLATE = lib
CONFIG += staticlib

DEFINES += SOILWIDGET_LIBRARY

INCLUDEPATH += ../mathFunctions ../soil ../utilities

SOURCES += \
        soilWidget.cpp \
    soilDbTools.cpp

HEADERS += \
        soilWidget.h \
    soilDbTools.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


