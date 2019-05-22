#-----------------------------------------------------
#
#   PRAGA GIS
#
#   this file is part of CRITERIA3D distribution
#
#-----------------------------------------------------

QT       += core gui network widgets

TARGET = PRAGA_GIS
TEMPLATE = app

INCLUDEPATH +=  ../PRAGA/shared ../mapGraphics  \
                ../crit3dDate ../mathFunctions ../gis  \
                ../shapeHandler ../shapeHandler/shapelib

CONFIG += debug_and_release

unix:{
    LIBS += -L../mapGraphics/release -lMapGraphics
}
win32:{
    CONFIG(debug, debug|release) {
         LIBS += -L../mapGraphics/debug -lMapGraphics
    } else {
        LIBS += -L../mapGraphics/release -lMapGraphics
    }
}

CONFIG(debug, debug|release) {
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../gis/debug -lgis
    LIBS += -L../shapeHandler/debug -lshapeHandler
} else {
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../mathFunctions/release -lmathFunctions
    LIBS += -L../gis/release -lgis
    LIBS += -L../shapeHandler/release -lshapeHandler
}


SOURCES += main.cpp\
    ../PRAGA/shared/colorLegend.cpp \
    mainWindow.cpp \
    gisProject.cpp \
    showProperties.cpp\
    dbfTableDialog.cpp \
    dbfNewColDialog.cpp \
    unitCropMap.cpp \
    ../PRAGA/shared/mapGraphicsShapeObject.cpp \
    ../PRAGA/shared/mapGraphicsRasterObject.cpp


HEADERS += \
    ../PRAGA/shared/colorLegend.h \
    mainWindow.h \
    gisProject.h \
    showProperties.h\
    dbfTableDialog.h \
    dbfNewColDialog.h \
    unitCropMap.h \
    ../PRAGA/shared/mapGraphicsShapeObject.h \
    ../PRAGA/shared/mapGraphicsRasterObject.h


FORMS += mainWindow.ui \
../PRAGA/shared/formInfo.ui

