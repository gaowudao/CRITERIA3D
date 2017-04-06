QT += core xml
QT -= gui

TARGET = WG
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../crit3dDate
LIBS += -L../crit3dDate/release -lcrit3dDate

INCLUDEPATH += ../mathFunctions
LIBS += -L../mathFunctions/release -lmathFunctions

HEADERS += \
    src/climate.h \
    src/fileUtility.h \
    src/parserXML.h \
    src/timeUtility.h \
    src/weathergenerator.h

SOURCES += \
    src/climate.cpp \
    src/fileUtility.cpp \
    src/main.cpp \
    src/parserXML.cpp \
    src/timeUtility.cpp \
    src/weatherGenerator.cpp

DISTFILES += \
    MOSES_WG.ini




