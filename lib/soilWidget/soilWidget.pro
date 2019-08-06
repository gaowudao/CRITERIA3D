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

INCLUDEPATH += ../mathFunctions ../specialMathFunctions ../soil ../utilities
unix:{
    INCLUDEPATH += /usr/include/qwt/
}

SOURCES += \
        soilWidget.cpp \
    soilDbTools.cpp \
    tabHorizons.cpp \
    tabWaterRetentionCurve.cpp \
    tabWaterRetentionData.cpp \
    tabHydraulicConductivityCurve.cpp \
    tableDelegate.cpp \
    tableDbOrModel.cpp \
    lineHorizont.cpp

HEADERS += \
        soilWidget.h \
    soilDbTools.h \
    tabHorizons.h \
    tabWaterRetentionCurve.h \
    tabWaterRetentionData.h \
    tabHydraulicConductivityCurve.h \
    tableDelegate.h \
    tableDbOrModel.h \
    lineHorizont.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:{
    include($$(QWT_ROOT)\features\qwt.prf)
}
unix:{
    include(/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qwt.prf)
}

