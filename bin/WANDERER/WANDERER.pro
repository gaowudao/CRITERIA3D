#------------------------------------------------------
#
#   WANDERER
#   simple gis for shapefile functions
#   This project is part of CRITERIA-3D distribution
#
#------------------------------------------------------


QT    += core gui network widgets

TEMPLATE = app
TARGET = WANDERER

INCLUDEPATH +=  ../../mapGraphics \
                ../../agrolib/crit3dDate ../../agrolib/mathFunctions ../../agrolib/gis  \
                ../../agrolib/graphics ../../agrolib/shapeHandler \
                ../../agrolib/shapeHandler/shapelib

CONFIG += debug_and_release


CONFIG(debug, debug|release) {
    LIBS += -L../../agrolib/graphics/debug -lgraphics
    LIBS += -L../../mapGraphics/debug -lMapGraphics
    LIBS += -L../../agrolib/shapeHandler/debug -lshapeHandler
    LIBS += -L../../agrolib/gis/debug -lgis
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../../agrolib/graphics/release -lgraphics
    LIBS += -L../../mapGraphics/release -lMapGraphics
    LIBS += -L../../agrolib/shapeHandler/release -lshapeHandler
    LIBS += -L../../agrolib/gis/release -lgis
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
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

