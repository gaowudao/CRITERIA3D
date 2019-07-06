#-----------------------------------------------------
#
#   PRAGA GIS
#   Simple gis for shapefile functions
#   This project is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += core gui network widgets

TARGET = PRAGA_GIS
TEMPLATE = app

INCLUDEPATH +=  ../PRAGA/shared   \
                ../../mapGraphics \
                ../../lib/crit3dDate ../../lib/mathFunctions ../../lib/gis  \
                ../../lib/shapeHandler ../../lib/shapeHandler/shapelib

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
    LIBS += -L../../lib/shapeHandler/debug -lshapeHandler
    LIBS += -L../../lib/gis/debug -lgis
    LIBS += -L../../lib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../../lib/shapeHandler/release -lshapeHandler
    LIBS += -L../../lib/gis/release -lgis
    LIBS += -L../../lib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../lib/mathFunctions/release -lmathFunctions
}


SOURCES += main.cpp\
    ../PRAGA/shared/colorLegend.cpp \
    ../PRAGA/shared/mapGraphicsShapeObject.cpp \
    ../PRAGA/shared/mapGraphicsRasterObject.cpp \
    ../PRAGA/shared/formInfo.cpp \
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
    ../PRAGA/shared/colorLegend.h \
    ../PRAGA/shared/mapGraphicsShapeObject.h \
    ../PRAGA/shared/mapGraphicsRasterObject.h \
    ../PRAGA/shared/formInfo.h \
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


FORMS += mainWindow.ui \
    ../PRAGA/shared/formInfo.ui

