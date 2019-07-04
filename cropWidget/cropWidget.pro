#----------------------------------------------------
#
#   Crop Widget
#   This project is part of CRITERIA-3D distribution
#
#----------------------------------------------------

QT  += widgets sql

TARGET = cropWidget
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions ../utilities ../soil ../crop ../gis ../meteo

SOURCES += \
    cropDbTools.cpp \
    dbToolsMOSES.cpp

HEADERS += \
    cropDbTools.h \
    dbToolsMOSES.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


