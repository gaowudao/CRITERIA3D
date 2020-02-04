#------------------------------------------------------
#
#   CRITERIA 1D-GEO
#   shapefile functions to run geographical version
#   of soil water balance 1D
#
#   This project is part of CRITERIA-3D distribution
#
#------------------------------------------------------


QT    += core gui network widgets sql

TEMPLATE = app
TARGET = CRITERIA_GEO

INCLUDEPATH +=  ../../mapGraphics \
                ../../agrolib/crit3dDate ../../agrolib/mathFunctions ../../agrolib/gis  \
                ../../agrolib/shapeHandler ../../agrolib/shapeHandler/shapelib \
                ../../agrolib/graphics ../../agrolib/project

CONFIG += debug_and_release


CONFIG(debug, debug|release) {
    LIBS += -L../../mapGraphics/debug -lMapGraphics
    LIBS += -L../../agrolib/shapeHandler/debug -lshapeHandler
    LIBS += -L../../agrolib/gis/debug -lgis
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions
} else {
    LIBS += -L../../mapGraphics/release -lMapGraphics
    LIBS += -L../../agrolib/shapeHandler/release -lshapeHandler
    LIBS += -L../../agrolib/gis/release -lgis
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
}


HEADERS += \
    ../../agrolib/graphics/mapGraphicsRasterObject.h \
    ../../agrolib/graphics/mapGraphicsShapeObject.h \
    ../../agrolib/graphics/colorLegend.h \
    ../../agrolib/project/formInfo.h \
    criteriaGeoProject.h \
    dialogSelectField.h \
    dialogShapeProperties.h \
    dialogUcm.h \
    dbfTableDialog.h \
    dbfNewColDialog.h \
    shapeUtilities.h \
    ucmDb.h \
    ucmUtilities.h \
    unitCropMap.h \
    shapeToRaster.h \
    zonalStatistic.h \
    gisObject.h \
    mainWindow.h

SOURCES += \
    ../../agrolib/graphics/mapGraphicsRasterObject.cpp \
    ../../agrolib/graphics/mapGraphicsShapeObject.cpp \
    ../../agrolib/graphics/colorLegend.cpp \
    ../../agrolib/project/formInfo.cpp \
    criteriaGeoProject.cpp \
    dialogSelectField.cpp \
    dialogShapeProperties.cpp \
    dialogUcm.cpp \
    mainWindow.cpp \
    dbfTableDialog.cpp \
    dbfNewColDialog.cpp \
    shapeUtilities.cpp \
    ucmDb.cpp \
    ucmUtilities.cpp \
    unitCropMap.cpp \
    shapeToRaster.cpp \
    zonalStatistic.cpp \
    gisObject.cpp \
    main.cpp


FORMS += \
    ../../agrolib/project/formInfo.ui \
    mainWindow.ui


