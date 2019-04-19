#-----------------------------------------------------
#
#   TEST of shapeHandler libray
#   this project is part of CRITERIA-3D distribution
#
#-----------------------------------------------------

QT  += widgets

TEMPLATE = app

SOURCES += main.cpp mainwindow.cpp dbfTableDialog.cpp \
    dbfNewColDialog.cpp

HEADERS += mainwindow.hpp dbfTableDialog.h \
    dbfNewColDialog.h

FORMS   += mainwindow.ui


CONFIG += debug_and_release


unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/TESTSHAPE
    } else {
        TARGET = release/TESTSHAPE
    }
}
win32:{
    TARGET = TESTSHAPE
}

INCLUDEPATH +=  ../shapeHandler ../shapeHandler/shapelib

unix:{
    LIBS += -L../shapeHandler/release -lshapeHandler -L../shapeDBF/release
}
win32:{
    CONFIG(debug, debug|release) {
        LIBS += -L../shapeHandler/debug -lshapeHandler -L../shapeDBF/debug
    } else {
        LIBS += -L../shapeHandler/release -lshapeHandler -L../shapeDBF/release
    }
}


