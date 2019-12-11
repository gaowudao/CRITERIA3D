#-----------------------------------------------------
#
#   CRITERIA3D
#   3D soil water balance
#   This project is part of CRITERIA-3D distribution
#
#-----------------------------------------------------

QT       += core gui network widgets sql xml 3dcore 3drender 3dextras

TARGET = CRITERIA3D
TEMPLATE = app


INCLUDEPATH +=  ./shared ../PRAGA/shared  \
                ../../mapGraphics \
                ../../lib/soilFluxes3D/header  \
                ../../lib/crit3dDate ../../lib/mathFunctions \
                ../../lib/crop ../../lib/soil ../../lib/meteo ../../lib/gis \
                ../../lib/interpolation ../../lib/solarRadiation  \
                ../../lib/soilWidget ../../lib/utilities  \
                ../../lib/dbMeteoPoints ../../lib/dbMeteoGrid \
                ../../lib/netcdfHandler ../../lib/project

unix:{
    INCLUDEPATH += /usr/include/qwt/
}
macx:{
    INCLUDEPATH += /usr/local/opt/qwt/lib/qwt.framework/Headers/
}

CONFIG += debug_and_release

unix:{
    LIBS += -L../../mapGraphics/release -lMapGraphics
}
win32:{
    CONFIG(debug, debug|release) {
         LIBS += -L../../mapGraphics/debug -lMapGraphics
    } else {
        LIBS += -L../../mapGraphics/release -lMapGraphics
    }
}

CONFIG(debug, debug|release) {
    LIBS += -L../../lib/project/debug -lproject
    LIBS += -L../../lib/netcdfHandler/debug -lnetcdfHandler
    LIBS += -L../../lib/dbMeteoGrid/debug -ldbMeteoGrid
    LIBS += -L../../lib/dbMeteoPoints/debug -ldbMeteoPoints
    LIBS += -L../../lib/soilWidget/debug -lsoilWidget
    LIBS += -L../../lib/crop/debug -lcrop
    LIBS += -L../../lib/soil/debug -lsoil
    LIBS += -L../../lib/utilities/debug -lutilities
    LIBS += -L../../lib/solarRadiation/debug -lsolarRadiation
    LIBS += -L../../lib/interpolation/debug -linterpolation
    LIBS += -L../../lib/meteo/debug -lmeteo
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/soilFluxes3D/debug -lsoilFluxes3D
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate

} else {
    LIBS += -L../../lib/project/release -lproject
    LIBS += -L../../lib/netcdfHandler/release -lnetcdfHandler
    LIBS += -L../../lib/dbMeteoGrid/release -ldbMeteoGrid
    LIBS += -L../../lib/dbMeteoPoints/release -ldbMeteoPoints
    LIBS += -L../../lib/soilWidget/release -lsoilWidget
    LIBS += -L../../lib/crop/release -lcrop
    LIBS += -L../../lib/soil/release -lsoil
    LIBS += -L../../lib/utilities/release -lutilities
    LIBS += -L../../lib/solarRadiation/release -lsolarRadiation
    LIBS += -L../../lib/interpolation/release -linterpolation
    LIBS += -L../../lib/meteo/release -lmeteo
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/soilFluxes3D/release -lsoilFluxes3D
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
}


SOURCES += mainwindow.cpp \
    ../PRAGA/shared/stationMarker.cpp \
    ../PRAGA/shared/mapGraphicsRasterObject.cpp \
    ../PRAGA/shared/colorLegend.cpp \
    shared/project3D.cpp \
    viewer3d.cpp \
    crit3dProject.cpp \
    main.cpp



HEADERS += mainwindow.h \
    ../PRAGA/shared/stationMarker.h \
    ../PRAGA/shared/mapGraphicsRasterObject.h \
    ../PRAGA/shared/colorLegend.h \
    shared/project3D.h \
    viewer3d.h \
    crit3dProject.h



FORMS += mainwindow.ui


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
