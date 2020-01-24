#------------------------------------------------------
#
#   CRITERIA_GEO
#   shapefile functions to run CRITERIA-1D
#   in a geographical area
#   This project is part of CRITERIA-3D distribution
#
#------------------------------------------------------


QT    += core gui network widgets

TEMPLATE = app
TARGET = CRITERIA_GEO

INCLUDEPATH +=  ../../mapGraphics \
                ../../agrolib/crit3dDate ../../agrolib/mathFunctions ../../agrolib/gis  \
                ../../agrolib/graphics ../../agrolib/shapeHandler \
                ../../agrolib/shapeHandler/shapelib ../../agrolib/project

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
    dialogSelectField.h \
    dialogShapeProperties.h \
    gisProject.h \
    dbfTableDialog.h \
    dbfNewColDialog.h \
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
    dialogSelectField.cpp \
    dialogShapeProperties.cpp \
    mainWindow.cpp \
    gisProject.cpp \
    dbfTableDialog.cpp \
    dbfNewColDialog.cpp \
    unitCropMap.cpp \
    shapeToRaster.cpp \
    zonalStatistic.cpp \
    gisObject.cpp \
    main.cpp


FORMS += \
    ../../agrolib/project/formInfo.ui \
    mainWindow.ui


