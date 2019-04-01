#-------------------------------------------------
#
# TEST of shapeHandler libray
#
#-------------------------------------------------

QT  += widgets

TARGET = SHAPES_TEST
TEMPLATE = app

SOURCES += main.cpp \
        mainwindow.cpp

HEADERS += mainwindow.hpp

FORMS   += mainwindow.ui

INCLUDEPATH +=  \
                ../shapeHandler         \
                ../shapeHandler/shapelib

win32:{
    CONFIG(debug, debug|release) {
        LIBS += -L../shapeHandler/debug -lshapeHandler
    } else {
        LIBS += -L../shapeHandler/release -lshapeHandler
    }
}

