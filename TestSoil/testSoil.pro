QT       += core gui widgets sql

TEMPLATE = app

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../soil/debug -lsoil
    LIBS += -L../soilWidget/debug -lsoilWidget
    LIBS += -L../utilities/debug -lutilities

} else {

    LIBS += -L../mathFunctions/release -lmathFunctions
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../soil/release -lsoil
    LIBS += -L../soilWidget/release -lsoilWidget
    LIBS += -L../utilities/release -lutilities
}

INCLUDEPATH += ../mathFunctions ../crit3dDate ../soil ../utilities ../soilWidget

SOURCES += \
        main.cpp
