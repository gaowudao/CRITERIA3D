#----------------------------------------------------
#
#   Soil Widget library
#   This project is part of CRITERIA-3D distribution
#
#----------------------------------------------------

QT  += widgets sql

TEMPLATE = lib
CONFIG += staticlib

CONFIG += debug_and_release

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/soilWidget
    } else {
        TARGET = release/soilWidget
    }
}
win32:{
    TARGET = soilWidget
}

INCLUDEPATH += ../mathFunctions ../soil ../utilities

SOURCES += \
        soilWidget.cpp \
    soilDbTools.cpp \
    horizonsTab.cpp

HEADERS += \
        soilWidget.h \
    soilDbTools.h \
    horizonsTab.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


