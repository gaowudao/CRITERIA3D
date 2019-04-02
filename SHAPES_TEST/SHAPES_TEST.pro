#-------------------------------------------------
#
# TEST of shapeHandler libray
#
#-------------------------------------------------

QT  += widgets

#TARGET = SHAPES_TEST
TEMPLATE = app

SOURCES += main.cpp \
        mainwindow.cpp

HEADERS += mainwindow.hpp

FORMS   += mainwindow.ui


CONFIG += debug_and_release


unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/SHAPES_TEST
    } else {
        TARGET = release/SHAPES_TEST
    }
}
win32:{
    TARGET = SHAPES_TEST
}


INCLUDEPATH +=  ../shapeHandler ../shapeHandler/shapelib

unix:{
    LIBS += -L../shapeHandler/release -lshapeHandler
}
win32:{
    CONFIG(debug, debug|release) {
        LIBS += -L../shapeHandler/debug -lshapeHandler
    } else {
        LIBS += -L../shapeHandler/release -lshapeHandler
    }
}

