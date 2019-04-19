QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

INCLUDEPATH += ../mathFunctions
INCLUDEPATH += ../crit3dDate
INCLUDEPATH += ../meteo
INCLUDEPATH += ../gis
CONFIG(release, debug|release) {

    LIBS += -L../mathFunctions/release -lmathFunctions
    LIBS += -L../crit3dDate/release -lcrit3dDate
    LIBS += -L../meteo/release -lmeteo
    LIBS += -L../gis/release -lgis
} else {

    LIBS += -L../mathFunctions/debug -lmathFunctions
    LIBS += -L../crit3dDate/debug -lcrit3dDate
    LIBS += -L../meteo/debug -lmeteo
    LIBS += -L../gis/debug -lgis
}


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    hops.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    hops.h
