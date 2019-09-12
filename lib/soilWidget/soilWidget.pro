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
unix:{
    INCLUDEPATH += /usr/include/qwt/
}

SOURCES += \
    barHorizon.cpp \
    curvePicker.cpp \
    soilTable.cpp \
    soilWidget.cpp \
    soilDbTools.cpp \
    tabHorizons.cpp \
    tabWaterRetentionCurve.cpp \
    tabWaterRetentionData.cpp \
    tabHydraulicConductivityCurve.cpp \
    tableDelegate.cpp \
    tableDelegateWaterRetention.cpp \
    curvePanner.cpp \
    dialogNewSoil.cpp

HEADERS += \
    barHorizon.h \
    curvePicker.h \
    soilTable.h \
    soilWidget.h \
    soilDbTools.h \
    tabHorizons.h \
    tabWaterRetentionCurve.h \
    tabWaterRetentionData.h \
    tabHydraulicConductivityCurve.h \
    tableDelegate.h \
    tableDelegateWaterRetention.h \
    curvePanner.h \
    tableWidgetItem.h \
    dialogNewSoil.h

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

