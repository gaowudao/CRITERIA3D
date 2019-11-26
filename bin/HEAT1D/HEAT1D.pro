#-----------------------------------------------------
#
#   HEAT1D
#   Test for soilFluxex3D library in a 1D domain
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += widgets core

TARGET = HEAT1D
TEMPLATE = app

INCLUDEPATH += ../../lib/crit3dDate
INCLUDEPATH += ../../lib/mathFunctions
INCLUDEPATH += ../../lib/gis
INCLUDEPATH += ../../lib/soilFluxes3D/header

unix:{
    INCLUDEPATH += /usr/include/qwt/
}


CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
} else {
    LIBS += -L../../lib/soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
}


SOURCES += main.cpp\
    mainwindow.cpp \
    heat1D.cpp \
    graphs.cpp

HEADERS  += heat1D.h \
    mainwindow.h \
    graphs.h

FORMS    += \
    mainwindow.ui

# insert here path of qwt.prf
win32:{
    include($$(QWT_ROOT)/features/qwt.prf)
}
unix:{
    include(/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/qwt.prf)
}
macx:{
    include(/usr/local/opt/qwt/features/qwt.prf)
}

