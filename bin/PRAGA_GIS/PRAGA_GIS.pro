#------------------------------------------------------
#
#   PRAGA GIS
#   Simple gis for shapefile functions
#   This project is part of CRITERIA-3D distribution
#
#------------------------------------------------------


QT    += core gui network widgets

TARGET = PRAGA_GIS
TEMPLATE = app

INCLUDEPATH += ../../mapGraphics ../../lib/graphics  \
               ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/gis  \
               ../../lib/shapeHandler ../../lib/shapeHandler/shapelib

CONFIG += debug_and_release


CONFIG(debug, debug|release) {
    LIBS += -L../../lib/graphics/debug -lgraphics
    LIBS += -L../../mapGraphics/debug -lMapGraphics
    LIBS += -L../../lib/shapeHandler/debug -lshapeHandler
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../../lib/graphics/release -lgraphics
    LIBS += -L../../mapGraphics/release -lMapGraphics
    LIBS += -L../../lib/shapeHandler/release -lshapeHandler
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
}


SOURCES += main.cpp\
    mainWindow.cpp \
    gisProject.cpp \
    showProperties.cpp\
    dbfTableDialog.cpp \
    dbfNewColDialog.cpp \
    unitCropMap.cpp \
    shapeToRaster.cpp \
    zonalStatistic.cpp \
    dbfNumericFieldsDialog.cpp \
    ucmDialog.cpp \
    gisObject.cpp

HEADERS += \
    mainWindow.h \
    gisProject.h \
    showProperties.h\
    dbfTableDialog.h \
    dbfNewColDialog.h \
    unitCropMap.h \
    shapeToRaster.h \
    zonalStatistic.h \
    dbfNumericFieldsDialog.h \
    ucmDialog.h \
    gisObject.h


FORMS += mainWindow.ui

