QT -= gui
QT += core sql xml

CONFIG += console
TEMPLATE = app

unix:{
    CONFIG(debug, debug|release) {
        TARGET = debug/csvToShape
    } else {
        TARGET = release/csvToShape
    }
}
win32:{
    TARGET = csvToShape
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += \
    csvToShapeProject.cpp \
    main.cpp \

HEADERS += \ \
    csvToShapeProject.h

INCLUDEPATH +=  ../../agrolib/crit3dDate ../../agrolib/mathFunctions ../../agrolib/utilities

CONFIG(debug, debug|release) {

    LIBS += -L../../agrolib/utilities/debug -lutilities
    LIBS += -L../../agrolib/crit3dDate/debug -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/debug -lmathFunctions

} else {

    LIBS += -L../../agrolib/utilities/release -lutilities
    LIBS += -L../../agrolib/crit3dDate/release -lcrit3dDate
    LIBS += -L../../agrolib/mathFunctions/release -lmathFunctions
} 
