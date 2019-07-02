QT       += core gui widgets sql

TEMPLATE = app

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    LIBS += -L../soilWidget/debug -lsoilWidget
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../utilities/debug -lutilities
    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../crit3dDate/debug -lcrit3dDate
} else {

    LIBS += -L../soilWidget/release -lsoilWidget
    LIBS += -L../soil/release -lsoil
    LIBS += -L../utilities/release -lutilities
    LIBS += -L../mathFunctions/release -lmathFunctions
    LIBS += -L../crit3dDate/release -lcrit3dDate
}

INCLUDEPATH += ../mathFunctions ../crit3dDate ../utilities ../soil ../soilWidget

SOURCES += \
        main.cpp


