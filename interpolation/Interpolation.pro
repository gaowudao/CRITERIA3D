#-------------------------------------------------
#
# CRITERIA3D
# interpolation library
#
#-------------------------------------------------

QT   -= core gui

TARGET = Interpolation
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../crit3dDate ../mathFunctions ../gis ../meteo

SOURCES += interpolation.cpp \
    interpolationSettings.cpp \
    interpolationPoint.cpp \
    kriging.cpp

HEADERS += interpolation.h \
    interpolationSettings.h \
    interpolationPoint.h \
    interpolationConstants.h \
    kriging.h


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
